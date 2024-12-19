import copy
import hashlib
import logging
import yaml
import os
import re
import shutil
import sys
import subprocess

# Create logging
logging.basicConfig(level=logging.INFO, format="%(levelname)s: %(message)s")


class ReleaseConfig:
    """
    Class to parse the release configuration yml file

    The yml file should have the following keys:

    - package-name:
        The name of the package
    - exclude:
        List of directories and files to exclude from the package.
        Relative to the core directory.
    - index-name:
        The name of the index file. Without ".json" extension.
    - server:
        The server information to upload the package.
        This is open to different specifications and approaches
        for the package and package index file hosting.
        Currently the only one supported is:

        Github
        ------
        The github server should have the following keys:
        - type: github
        - owner: The owner of the repository
        - repo: The repository name

        From this information we can generate:
            - The url to download the package.
            - The url of the package index json file.


    Optional keys:

    - include:
        Specific files or subdirectories to include from any of the
        directories in the exclude list.

        This includes will be added after the exclude list is processed.
        Therefore, if a file is in both the exclude and include list,
        it will be included.

    - index-template:
        The template for the package index file.
        If not provided, the expected default value is:
        "package/<index-name>.template.json"

    Reference example yml file:

    ```
    package-name: arduino-core-vendor-product
    excludes:
        - .github
        - docs
        - extras
        - package
        - tests
        - .codespellrc
        - .gitattributes
        - .gitignore
        - .gitmodules
        - .pre-commit-config.yaml
        - .readthedocs.yml
        - CODE_OF_CONDUCT.md
        - CONTRIBUTING.md
        - CODE_CONVENTIONS.md
        - .clang-format
        - .clang-tidy
    index-name: package_vendor_product_index
    server:
        type: github
        owner: vendor-owner
        repo: arduino-core-repo-name
    ```
    """

    def __init__(self, config_yml):
        """
        Parse the configuration yml file and
        set the class attributes.
        """
        self.config_file = config_yml
        self.config = self.__parse_config()

        self.package_name = self.config["package-name"]
        self.exclude = self.config["exclude"]
        self.include = self.config["include"]
        self.index_name = self.config["index-name"]
        self.server = self.config["server"]

        if "index-template" in self.config:
            self.index_template = self.config["index-template"]
        else:
            self.index_template = os.path.join(
                "package", self.index_name + ".template.json"
            )

        logging.info("Core release configuration loaded:\n\n" + self.__str__())

    def __str__(self):
        """Use yaml dump to generate a string"""
        return yaml.dump(self.config)

    """ Private methods """

    def __parse_config(self):
        """Parse the configuration yml file"""
        try:
            with open(self.config_file, "r") as f:
                config = yaml.safe_load(f)
        except FileNotFoundError:
            logging.error(f'Release config file "{self.config_file}" not found')
            sys.exit(1)

        return config


class PackageVersion:

    def __init__(self):

        # Get the git sha or tag
        try:
            git_tag = (
                subprocess.check_output(["git", "describe", "--tags"])
                .strip()
                .decode("utf-8")
            )
            git_sha = (
                subprocess.check_output(["git", "rev-parse", "HEAD"])
                .strip()
                .decode("utf-8")
            )
        except subprocess.CalledProcessError as e:
            logging.error(
                "Git process failed. Make sure this the core git repo and git is installed."
            )
            sys.exit(1)

        self.tag = self.__strip_prefix_from_version(git_tag)
        self.commit = git_sha

    """ Private methods """

    def __strip_prefix_from_version(self, version):
        """
        Strips 'v' or 'V' prefix from version.
        """
        return re.sub(r"[vV]", "", version)


class CorePackage:
    """
    Class to create the core package.

    It will create a directory with the package name and version,
    copy the necessary files and directories, and zip the package.

    It provides the necessary methods to get the package archive
    name, size, and sha256 hash.
    """

    def __init__(
        self,
        package_name,
        package_version,
        exclude,
        include,
        core_root_path,
        build_path,
    ):
        """
        The constructor directly creates the package.

        Args:
         - package_name: The name of the package.
         - version: The version of the package.
         - exclude: List of directories and files to exclude from the package.
         - include: List of directories and files to include in the package.
         - core_root_path: The path of the core.
         - build_path: The path to build the package.

        """
        self.core_root_path = core_root_path
        self.build_path = build_path

        self.__pack(package_name, package_version, exclude, include)

    def get_archive_name(self):
        """
        Returns the name of the archived package.
        """
        return str(self.pkg_name) + ".zip"

    def get_size(self):
        """
        Returns the size of the package in bytes.
        """
        return os.path.getsize(self.pkg_path)

    def get_sha256(self):
        """
        Returns the sha256 hash of the package.
        """
        with open(self.pkg_path, "rb") as f:
            bytes = f.read()
            hash = hashlib.sha256(bytes).hexdigest()
        return hash

    def clean(self):
        """
        Cleans the package directory.
        """
        logging.info(f"Cleaning package directory: {self.pkg_path}...")
        shutil.rmtree(self.pkg_path)

    """ Private methods """

    def __make_package_dir(self, package_name, version_tag):
        """
        Creates the package directory.
        The name is the package name concatenated with the
        version with a '-' separator.

        If the package directory already exists, it will
        overwrite it.

        Args:
            - package_name: The name of the package.
            - version_tag: The version tag of the package.
        """
        pkg_name_w_ver = package_name + "-" + version_tag
        pkg_path = os.path.join(self.build_path, pkg_name_w_ver)

        self.pkg_name = pkg_name_w_ver
        self.pkg_path = os.path.join(self.build_path, self.pkg_name)
        logging.info(f"Creating package directory: {self.pkg_path}...")

        if os.path.exists(pkg_path):
            logging.warning(
                f"Package directory {pkg_path} already exists. Overwriting it..."
            )
            shutil.rmtree(pkg_path)
        os.makedirs(pkg_path)

    def __copy_includes(self, exclude, include):
        """ """
        # From includes, distinguish between directories and files
        # include_dirs = []
        # include_files = []
        # for include in includes:
        #     if os.path.isdir(os.path.join(self.core_root_path, include)):
        #         include_dirs.append(include)
        #     else:
        #         include_files.append(include)

        # for dir in include_dirs:
        #     shutil.copytree(
        #         os.path.join(self.core_root_path, dir), os.path.join(self.pkg_path, dir)
        #     )
        # for file in include_files:
        #     shutil.copy(
        #         os.path.join(self.core_root_path, file), os.path.join(self.pkg_path, file)
        #     )
        pass

    def __add_version(self, package_version):
        """
        Creates a file with the version information and
        add it to the package.

        Args:
            - package_version: The package version object.
        """

        # Create the version file
        logging.info(f"Adding version information to package...")
        logging.info(f"Tag: {package_version.tag}")
        logging.info(f"Commit: {package_version.commit}")
        version_file = os.path.join(self.pkg_path, ".version")
        with open(version_file, "w") as f:
            f.write(f"tag:    {package_version.tag}\n")
            f.write(f"commit: {package_version.commit }\n")

    def __zip(self):
        """
        Zips the package directory.
        """
        shutil.make_archive(self.pkg_path, "zip", self.build_path, self.pkg_name)

    def __pack(self, package_name, package_version, exclude, include):
        """
        Creates the package.

        Args:
            - package_name: The name of the package.
            - package_version: The package version object.
            - exclude: List of directories and files to exclude from the package.
            - include: List of directories and files to include in the package.
        """
        self.__make_package_dir(package_name, package_version.tag)
        self.__copy_includes(exclude, include)
        self.__add_version(package_version)
        self.__zip()


def build_release_assets(rel_conf_yml, core_root_path, pkg_build_path):
    """Build the package release assets. Consisting of:
    - The package sources
    - The package index file

    Args:
        rel_conf_yml (str): The path to the release configuration yml file.

    """

    # Load the release configuration from the yml file
    release_config = ReleaseConfig(rel_conf_yml)

    # Get the package version information
    package_version = PackageVersion()

    # Create the package directory
    core_package = CorePackage(
        release_config.package_name,
        package_version,
        release_config.exclude,
        release_config.include,
        core_root_path,
        pkg_build_path,
    )

    # Remove the package files
    # core_package.clean()


if __name__ == "__main__":

    core_root_path = os.path.relpath(os.getcwd())
    build_dir_name = "pkg_build"
    pkg_assets_build_path = os.path.join(core_root_path, build_dir_name)
    build_release_assets("release-config.yml", core_root_path, pkg_assets_build_path)
