import argparse
import copy
import hashlib
import json
import logging
import yaml
import os
import re
import requests
import shutil
import sys
import subprocess

# Create logging
logging.basicConfig(level=logging.WARNING, format="%(levelname)s: %(message)s")


class ReleaseConfig:
    """
    Class to parse the release configuration yml file

    The yml file needs the following keys:

    - package-name:
        The name of the package
    - include:
        List of files and directories to include from any of the
        directories in the exclude list.
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
        - owner: The owner of the repository (user name or organization)
        - repo: The repository name

        From this information we can generate:
            - The url to download the package.
            - The url of the package index json file.


    Optional keys:

    - exclude:
        Specific files or subdirectories to exclude from any of the
        directories in the include list.

        This exclude list will be added after the include list is
        processed. Therefore, if a file is in both the exclude and
        include list, it will be excluded.

    - index-template:
        The template for the package index file.
        If not provided, the expected default value is:
        "package/<index-name>.template.json"

    Reference example yml file:

    ```
    package-name: arduino-core-vendor-product
    include:
        - cores
        - libraries
        - variants
        - boards.txt
        - platform.txt
        - LICENSE.md
        - README.md
        - post_install.sh
    index-name: package_vendor_product_index
    server:
        type: github
        owner: vendor-owner
        repo: arduino-core-repo-name
    ```
    """

    def __init__(self, config_yml):
        """
        Parses the configuration yml file and
        set the class attributes.

        Args:
            - config_yml: The path to the release configuration yml file.
        """
        self.config_file = config_yml
        self.config = self.__parse_config()

        self.package_name = self.config["package-name"]
        self.include = self.config["include"]
        self.index_name = self.config["index-name"]
        self.server = self.config["server"]

        if "exclude" in self.config:
            self.exclude = self.config["exclude"]
        else:
            self.exclude = []

        if "index-template" in self.config:
            self.index_template = self.config["index-template"]
        else:
            self.index_template = os.path.join(
                "package", self.index_name + ".template.json"
            )

        logging.info("Core release configuration loaded.\n\n" + self.__str__())

    def __str__(self):
        """Uses yaml dump to generate a string"""
        return yaml.dump(self.config)

    """ Private methods """

    def __parse_config(self):
        """Parses the configuration yml file"""
        try:
            with open(self.config_file, "r") as f:
                config = yaml.safe_load(f)
        except FileNotFoundError:
            logging.error(f'Release config file "{self.config_file}" not found')
            sys.exit(1)

        return config


class PackageVersion:
    """
    Class to handle the package version based on git versioning and
    the Arduino core metafiles.
    """

    def __init__(self):
        """
        The constructor sets the git tag and commit sha.

        Requires git to be installed.
        """

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

    def check_consistency(self, platform_txt):
        """
        The version consistency check is done by comparing the git
        versioning and the one in platform.txt.

        These need to be matching in a definitive release.

        Args:
            - platform_txt: The path to the platform.txt file.
        """

        # Get the version from the platform.txt
        with open(platform_txt, "r") as f:
            lines = f.readlines()
            for line in lines:
                if "version=" in line:
                    platform_version = line.split("=")[1].strip()
                    break

        # Check if the git tag is the same as the platform version
        if self.tag != platform_version:
            logging.error(
                f"Git tag version {self.tag} does not match the platform version {platform_version}"
            )
            sys.exit(1)

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
        include,
        exclude,
        core_root_path,
        build_path,
    ):
        """
        The constructor directly creates the package.

        Args:
         - package_name: The name of the package.
         - version: The version of the package.
         - include: List of directories and files to include in the package.
         - exclude: List of directories and files to exclude from the package.
         - core_root_path: The path of the core.
         - build_path: The path to build the package.

        """
        self.core_root_path = core_root_path
        self.build_path = build_path

        self.__pack(package_name, package_version, include, exclude)

    def get_archive_name(self):
        """
        Returns the name of the archived package.
        """
        return str(self.pkg_name) + ".zip"

    def get_size(self):
        """
        Returns the size of the archived package in bytes.
        """
        return os.path.getsize(self.pkg_archive_path)

    def get_sha256(self):
        """
        Returns the sha256 hash of the archived package.
        """
        with open(self.pkg_archive_path, "rb") as f:
            bytes = f.read()
            hash = hashlib.sha256(bytes).hexdigest()
        return hash

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
        self.pkg_archive_path = self.pkg_path + ".zip"
        logging.info(f"Creating package directory: {self.pkg_path}")

        if os.path.exists(pkg_path):
            logging.warning(
                f"Package directory {pkg_path} already exists. Overwriting it"
            )
            shutil.rmtree(pkg_path)
        os.makedirs(pkg_path)

    def __copy_includes(self, include, exclude):
        """
        Includes the necessary files and directories in the package.

        The exclude list is added after the include list is processed.
        Therefore, if a file or dir is in both the exclude and include list,
        it will be excluded.

        Args:
            - include: List of directories and files to include in the package.
            - exclude: List of directories and files to exclude from the package.
        """
        logging.info(f"Copying includes: {include}")

        # From includes, distinguish between directories and files
        include_dirs = []
        include_files = []
        for item in include:
            if os.path.isdir(os.path.join(self.core_root_path, item)):
                include_dirs.append(item)
            else:
                include_files.append(item)

        for dir in include_dirs:
            if not os.path.exists(os.path.join(self.core_root_path, dir)):
                logging.warning(f'Directory "{dir}" does not exist')
                continue
            shutil.copytree(
                os.path.join(self.core_root_path, dir), os.path.join(self.pkg_path, dir)
            )

        for file in include_files:
            if not os.path.exists(os.path.join(self.core_root_path, file)):
                logging.warning(f'File "{file}" does not exist')
                continue

            shutil.copy(
                os.path.join(self.core_root_path, file),
                os.path.join(self.pkg_path, file),
            )

        # Exclude the necessary files and directories
        logging.info(f"Explicit excluded dir and files: {exclude}")
        for item in exclude:
            if os.path.isdir(os.path.join(self.pkg_path, item)):
                shutil.rmtree(os.path.join(self.pkg_path, item))
            else:
                os.remove(os.path.join(self.pkg_path, item))

    def __add_version(self, package_version):
        """
        Creates a file with the version information and
        add it to the package.

        Args:
            - package_version: The package version object.
        """
        version_file = os.path.join(self.pkg_path, ".version")
        with open(version_file, "w") as f:
            f.write(f"tag:    {package_version.tag}\n")
            f.write(f"commit: {package_version.commit }\n")

        logging.info(f"Added version information to package in {version_file}")
        logging.info(f"  - tag:    {package_version.tag}")
        logging.info(f"  - commit: {package_version.commit}")

    def __zip(self):
        """
        Zips the package directory.
        """
        shutil.make_archive(self.pkg_path, "zip", self.build_path, self.pkg_name)

    def __pack(self, package_name, package_version, include, exclude):
        """
        Creates the package.

        Args:
            - package_name: The name of the package.
            - package_version: The package version object.
            - include: List of directories and files to include in the package.
            - exclude: List of directories and files to exclude from the package.
        """
        self.__make_package_dir(package_name, package_version.tag)
        self.__copy_includes(include, exclude)
        self.__add_version(package_version)
        self.__zip()


class PackageIndex:
    """
    Class to create the package index json file.
    """

    def __init__(
        self,
        archive_name,
        version,
        size,
        sha256,
        pckg_index_name,
        pckg_index_template,
        server,
        inc_previous_release,
        core_root_path,
        build_path,
    ):
        """
        The constructor creates the package index json file.

        Args:
            - archive_name: The name of the package archive (including .zip extension).
            - version: The version of the package.
            - size: The size of the package in bytes.
            - sha256: The sha256 hash of the package.
            - pckg_index_name: The name of the package index file.
            - pckg_index_template: The package index template json file.
            - server: The server information to upload the package.
            - inc_previous_release: Include previous releases in the package index.
            - core_root_path: The path to the Arduino core.
            - build_path: The path to build the package index json file.

        """
        self.archive_name = archive_name
        self.version = version
        self.size = size
        self.sha256 = sha256
        self.pckg_index_name = pckg_index_name
        self.pckg_index_template = pckg_index_template
        self.inc_previous_releases = inc_previous_release
        self.server = server
        self.core_root_path = core_root_path
        self.build_path = build_path

        self.__build_package_index()

    def clean(self):
        """
        Cleans the package index file.
        """
        logging.info(f"Cleaning package index file: {self.pckg_index_name}.json")
        os.remove(os.path.join(self.build_path, self.pckg_index_name + ".json"))

    """ Private methods """

    def __set_package_download_url(self):
        """
        Sets the package download url based on the server info.

        Supported servers:
         - Github: With required keys
         {
            "type": "github"
            "owner": "github_owner"
            "repo": "github_repo_name"
        }
        """
        if self.server["type"] == "github":

            package_url = (
                "https://github.com/"
                + str(self.server["owner"])
                + "/"
                + str(self.server["repo"])
                + "/releases/download/"
                + str(self.version)
                + "/"
                + str(self.archive_name)
            )

        else:
            logging.error(f"Server NOT supported")
            sys.exit(1)

        logging.info(f'Setting package download url to "{package_url}"')

        return package_url

    def __get_latest_release_package_index(self):
        """
        Gets the latest release package index from the server.

        Supported servers:
            - Github

        Returns an empty index if the package index file does not exist.
        """
        # Create the url based on the server
        if self.server["type"] == "github":
            url = (
                "https://github.com/"
                + str(self.server["owner"])
                + "/"
                + str(self.server["repo"])
                + "/"
                + "releases/latest/download/"
                + str(self.pckg_index_name)
                + ".json"
            )
        else:
            logging.error("Server NOT supported.")
            sys.exit(1)

        logging.info(f'Retrieving the latest package index from "{url}"')
        response = requests.get(url)

        if response.status_code == 200:
            package_index = response.json()
        elif response.status_code == 404:
            # If the file does not exist
            # (and assuming download url is correct),
            # we can consider this is be the first release of
            # the core.
            # We return an empty index, as the platforms will be
            # appended to the new created package index, it will
            # not add any modifications to the new created index
            package_index = {"packages": [{"platforms": []}]}
            logging.warning("Previous latest package index file not found.")
        else:
            logging.error(
                f"Could not retrieve the latest package index, due to request error {response.status_code}"
            )

        return package_index

    def __get_package_index_template_json(self):
        """
        Retrieves and loads the package index json file template
        prefilled with the constant core information and tools.
        """
        logging.info(
            f'Loading package index template from "{self.pckg_index_template}"'
        )
        with open(
            os.path.join(self.core_root_path, self.pckg_index_template),
            "r",
        ) as f:
            pck_index_template = json.load(f)

        return pck_index_template

    def __insert_platform(self, package_index):
        """
        Inserts the new release platform into the package index.
        It will add the new platform data to the first package in the index.

        Args:
            - package_index: Package index based on the package index template.
        """
        platform = package_index["packages"][0]["platforms"][0]
        platform["version"] = self.version
        platform["archiveFileName"] = self.archive_name
        platform["url"] = self.__set_package_download_url()
        platform["checksum"] = "SHA-256:" + self.sha256
        platform["size"] = self.size

        logging.info(f"Added new platform to package index: {json.dumps(platform)}")

    def __add_previous_platforms(self, package_index):
        """
        Retrieves and adds any previous releases to package index.
        This a list contained in the "platforms" key.

        Args:
            - package_index: Package index with only current release version.
        """
        if self.inc_previous_releases:
            # Retrieve the latest release package index
            latest_release_package_index = self.__get_latest_release_package_index()
            # The existing platform contains all the previous core package releases.
            latest_platforms = copy.deepcopy(
                latest_release_package_index["packages"][0]["platforms"]
            )
            # Prepend the new platform to the existing platforms.
            package_index["packages"][0]["platforms"].extend(latest_platforms)

            logging.info("Added previous platforms to package index")

    def __make_package_index_file(self, package_index):
        """
        Makes the package index json file

        Args:
            - package_index: Package index completed with all required data.
        """

        logging.info(f"Creating package index file: {self.pckg_index_name}.json")
        pkg_index_json_obj = json.dumps(package_index, indent=2)
        # Write the package index json file
        pkg_index_w_path = os.path.join(self.build_path, self.pckg_index_name + ".json")
        with open(pkg_index_w_path, "w") as pkg_file:
            pkg_file.write(pkg_index_json_obj)

    def __build_package_index(self):
        """
        Builds the package index file in the following stages:
            - Gets the template package index
            - Adds the current release version (platform)
            - Includes the previous existing releases
            - Creates the final package index json file.
        """
        package_index = self.__get_package_index_template_json()
        self.__insert_platform(package_index)
        self.__add_previous_platforms(package_index)
        self.__make_package_index_file(package_index)


def build_release_assets(
    rel_conf_yml,
    core_root_path,
    pkg_build_path,
    ver_check=True,
    inc_previous_releases=True,
):
    """
    Builds the package release assets. Consisting of:
        - The package sources
        - The package index file

    Args:
        - rel_conf_yml: The path to the release configuration yml file.
        - core_root_path: The path to the arduino core root directory.
        - pkg_build_path: The path to build the package.
        - ver_check: Assert that versions across core metafiles and git matches.
        - inc_previous_releases: Include previous releases in the package index.
    """

    # Load the release configuration from the yml file
    release_config = ReleaseConfig(rel_conf_yml)

    # Get the package version information
    package_version = PackageVersion()

    # Validate the version consistency
    if ver_check:
        package_version.check_consistency(os.path.join(core_root_path, "platform.txt"))

    # Create the package directory
    core_package = CorePackage(
        release_config.package_name,
        package_version,
        release_config.include,
        release_config.exclude,
        core_root_path,
        pkg_build_path,
    )

    # Create the package index manifest
    package_index = PackageIndex(
        core_package.get_archive_name(),
        package_version.tag,
        core_package.get_size(),
        core_package.get_sha256(),
        release_config.index_name,
        release_config.index_template,
        release_config.server,
        inc_previous_releases,
        core_root_path,
        pkg_build_path,
    )


class PackageParser:
    """
    Class to parse the command line arguments for the core package release script.
    """

    def __init__(self):
        """
        The constructor creates the argument parser and parses the arguments.
        """
        # Get the script name without .py extension
        self.package_tool_name = os.path.splitext(os.path.basename(__file__))[0]
        self.package_tool_version = "0.1.0"
        self.__create_parser()

        args = self.parser.parse_args(namespace=argparse.Namespace(package_parser=self))
        args.func(args)

    """ Private class methods """

    class __ver_action(argparse.Action):
        def __init__(self, option_strings, dest, **kwargs):
            super().__init__(
                option_strings, dest, nargs=0, default=argparse.SUPPRESS, **kwargs
            )

        def __call__(self, parser, namespace, values, option_string=None):
            # Retrieve the package_parser object from the namespace
            package_parser = getattr(namespace, "package_parser", None)
            print(
                package_parser.package_tool_name
                + " version: "
                + package_parser.package_tool_version
            )
            parser.exit()

    def __main_parser_func(self, args):
        """
        Main parser function to build the package release assets.
        """
        # If not provided, set the default values
        # to "package/config.yml" in the arduino core root path
        if args.config_yml is None:
            args.config_yml = os.path.join(args.root_path, "package", "config.yml")

        if args.build_path is None:
            args.build_path = os.path.join(args.root_path, "build")

        if args.verbose:
            # Enable logging info
            logging.getLogger().setLevel(logging.INFO)

            print(f"Arduino core root path:    {args.root_path}")
            print(f"Build path:                {args.build_path}")
            print(f"Package config yml:        {args.config_yml}")
            print(f"Version check:             {args.no_version_check}")
            print(f"Include previous releases: {args.no_previous_releases}")

        build_release_assets(
            args.config_yml,
            args.root_path,
            args.build_path,
            args.no_version_check,
            args.no_previous_releases,
        )

    def __parser_clean_func(self, args):
        """
        Remove the content of the build directory.
        """
        if args.build_path is None:
            args.build_path = os.path.join(args.root_path, "build")

        # if existing, remove the build directory
        if os.path.exists(args.build_path):
            logging.info(f"Cleaning build directory: {args.build_path}")
            shutil.rmtree(args.build_path)

    def __create_parser(self):
        """
        Creates the argument parser for the core package release script.
        """
        self.parser = argparse.ArgumentParser(
            description="Packaging of Arduino core release assets"
        )

        # Argument for version
        self.parser.add_argument(
            "-v",
            "--version",
            action=self.__ver_action,
            help=self.package_tool_name + " version",
        )

        # Argument for package config yaml file
        self.parser.add_argument(
            "-c",
            "--config-yml",
            type=str,
            default=None,
            help='The path to the release configuration yml file. Default is the "package/config.yml" file in the arduino core path',
        )

        # Argument for core root path
        self.parser.add_argument(
            "-r",
            "--root-path",
            type=str,
            default=os.getcwd(),
            help="Path to the arduino core root directory. Default is the current directory",
        )

        # Argument for package build path
        self.parser.add_argument(
            "-b",
            "--build-path",
            type=str,
            default=None,
            help='Path to build the package. Default is the "build/" directory in the arduino core path',
        )

        # Argument for version check
        self.parser.add_argument(
            "--no-version-check",
            action="store_false",
            default=True,
            help="Do not check the version consistency.",
        )

        # Argument for previous releases
        self.parser.add_argument(
            "--no-previous-releases",
            action="store_false",
            default=True,
            help="Do not include previous releases in the package index",
        )

        # Argument for verbose output
        self.parser.add_argument(
            "--verbose",
            action="store_true",
            default=False,
            help="Verbose output",
        )

        # Set the main parser function
        self.parser.set_defaults(func=self.__main_parser_func)

        # Subparsers
        subparsers = self.parser.add_subparsers()

        # Clean subparser
        parser_clean = subparsers.add_parser(
            "clean", description="Clean the  package index file"
        )
        parser_clean.set_defaults(func=self.__parser_clean_func)

        # Argument for core root path
        parser_clean.add_argument(
            "-r",
            "--root-path",
            type=str,
            default=os.getcwd(),
            help="Path to the arduino core root directory. Default is the current directory",
        )

        # Argument for build path
        parser_clean.add_argument(
            "-b",
            "--build-path",
            type=str,
            default=None,
            help='Path to build the package. Default is the "build/" directory in the arduino core path',
        )


if __name__ == "__main__":

    package_parser = PackageParser()
