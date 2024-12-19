import argparse
import copy
import hashlib
import json
import logging
import os
import re
import yaml
import requests
import shutil

# Create logging
logging.basicConfig(level=logging.INFO, format="%(levelname)s: %(message)s")

core_release_version = "0.0.0"

# psoc_ino_root_path = os.path.relpath(os.getcwd())
# build_dir_name = "pkg_build"
# # pkg_assets_build_path = os.path.join(psoc_ino_root_path, build_dir_name)

# package_name = "arduino-core-psoc"
# version="v0.1.1"
# includes = ["cores", "tools", os.path.join("extras","mtb-libs"), "examples", "variants",
#         "boards.txt",
#         "platform.txt",
#         "LICENSE.md",
#         "README.md",
#         "post_install.sh",
#     ]

# # Path to this file
# script_dir = os.path.dirname(os.path.abspath(__file__))
# # The core root path is the parent dir of the script path
# core_root_path = os.path.dirname(script_dir)
# # The build path is the parent dir of the core root path
# build_path = core_root_path


def change_dir_to_script_location():
    script_dir = os.path.dirname(os.path.abspath(__file__))
    os.chdir(script_dir)
    print(os.getcwd())


def __strip_prefix_from_version(version):
    """
    Strips 'v' or 'V' prefix from version.
    """
    return re.sub(r"[vV]", "", version)


# def mkdir_package_dir(version):
#     semver = strip_prefix_from_version(version)
#     pkg_name = package_name + "-" + semver
#     pkg_build_path = os.path.join(pkg_assets_build_path, pkg_name)
#     if os.path.exists(pkg_build_path):
#         shutil.rmtree(pkg_build_path)
#     os.makedirs(pkg_build_path)
#     return pkg_name


# def get_package_size(pkg):
#     return os.path.getsize(pkg)


# def get_package_sha256(pkg):
#     with open(pkg, "rb") as f:
#         bytes = f.read()
#         hash = hashlib.sha256(bytes).hexdigest()
#     return hash


# def build_package(pkg_name):
#     pkg_build_path = os.path.join(pkg_assets_build_path, pkg_name)
#     dirs_to_copy = ["cores", "tools", "extras/mtb-libs", "examples", "variants"]
#     files_to_copy = [
#         "boards.txt",
#         "platform.txt",
#         "LICENSE.md",
#         "README.md",
#         "post_install.sh",
#     ]
#     for dir in dirs_to_copy:
#         shutil.copytree(
#             os.path.join(psoc_ino_root_path, dir), os.path.join(pkg_build_path, dir)
#         )
#     for file in files_to_copy:
#         shutil.copy(
#             os.path.join(psoc_ino_root_path, file), os.path.join(pkg_build_path, file)
#         )


# def zip_package(pkg_name):
#     pkg_build_path = os.path.join(pkg_assets_build_path, pkg_name)
#     shutil.make_archive(pkg_build_path, "zip", pkg_assets_build_path, pkg_name)


class PackageURL:

    def __init__(self, server, archive_name, version):
        self.server = server
        self.archive_name = archive_name
        self.version = version

    # def get_package_url(self):

    #     if self.server == "github":

    #         package_url = "https://github.com/"
    #         + str(self.server["owner"]) + "/"
    #         + str(self.server["repo"]) + "/releases/download/"
    #         + str(self.version)
    #         + "/"
    #         + str(self.archive_name)

    #     elif self.server == "local":
    #         package_url = "http://localhost:8000/" + self.archive_name

    #     else:
    #         raise ValueError("Server not supported")

    #     return package_url

    # def get_latest_release_package_index(self):

    #     package_index = {"packages": [{"platforms": []}]}

    #     if self.server == "github":
    #         response = requests.get(
    #         "https://github.com/"
    #         + self.server["owner"]
    #         + "/"
    #         + self.server["repo"]
    #         + "/"
    #         + "releases/latest/download/"
    #         + self.package_index_name
    #         + ".json"
    #         )

    #         # If the file does not exist
    #         # this is the first release
    #         # if response.status_code == 404:
    #         #     # We return an empty index, as the platforms will be
    #         #     # appended to the new created package index, it will
    #         #     # not add any modifications to the new created index
    #         #     package_index =
    #         if response.status_code == 200:
    #             package_index= response.json()

    #     return package_index


class PackageIndex:

    def __init__(
        self,
        archive_name,
        version,
        size,
        sha256,
        pckg_index_template,
        core_root_path,
        build_path,
        server,
    ):
        self.server = server
        self.archive_name = archive_name
        self.version = __strip_prefix_from_version(version)
        self.size = size
        self.sha256 = sha256
        self.core_root_path = core_root_path
        self.build_path = build_path
        self.pckg_index_template = pckg_index_template

        self.__build_package_index()

    """ Private methods """

    def __set_package_download_url(self):

        if self.server == "github":

            package_url = "https://github.com/"
            +str(self.server["owner"])
            +"/"
            +str(self.server["repo"])
            +"/releases/download/"
            +str(self.version)
            +"/"
            +str(self.archive_name)

        elif self.server == "local":
            package_url = "http://localhost:8000/" + self.archive_name

        else:
            raise ValueError("Server not supported")

        return package_url

    def __get_latest_release_package_index(self):

        package_index = {"packages": [{"platforms": []}]}

        if self.server == "github":
            response = requests.get(
                "https://github.com/"
                + self.server["owner"]
                + "/"
                + self.server["repo"]
                + "/"
                + "releases/latest/download/"
                + self.package_index_name
                + ".json"
            )

            # If the file does not exist
            # this is the first release
            # if response.status_code == 404:
            #     # We return an empty index, as the platforms will be
            #     # appended to the new created package index, it will
            #     # not add any modifications to the new created index
            #     package_index =
            if response.status_code == 200:
                package_index = response.json()

        return package_index

    def __get_package_index_template_json(self):
        with open(
            os.path.join(self.core_root_path, self.package_index_template),
            "r",
        ) as f:
            data = json.load(f)
        return data

    def __insert_platform(self, package_index):
        """
        Inserts the new release platform into the package index.
        It will add the new platform data to the first package in the index.
        """
        platform = package_index["packages"][0]["platforms"][0]
        platform["version"] = self.version
        platform["archiveFileName"] = self.archive_name
        platform["url"] = self.__set_package_download_url()
        platform["checksum"] = "SHA-256:" + self.sha256
        platform["size"] = self.size

    def __make_package_index_file(self, package_index):

        pkg_index_json_obj = json.dumps(package_index, indent=2)
        pkg_index_w_path = os.path.join(self.built_path, "package_psoc_index.json")
        with open(pkg_index_w_path, "w") as pkg_file:
            pkg_file.write(pkg_index_json_obj)

    def __build_package_index(self):
        package_index = self.__get_package_index_template_json()
        self.__insert_platform(package_index)

        if self.server != "local":
            # If the server is not local, we need to get the latest release package index
            # and prepend the new platform to the existing platforms.
            # The existing platform conform all the previous core package releases.
            latest_release_package_index = self.__get_latest_release_package_index()
            latest_platforms = copy.deepcopy(
                latest_release_package_index["packages"][0]["platforms"]
            )
            package_index["packages"][0]["platforms"].extend(latest_platforms)

        self.__make_package_index_file(package_index)


# def get_latest_package_index_json():
#     response = requests.get(
#         "https://github.com/Infineon/arduino-core-psoc/releases/latest/download/package_psoc_index.json"
#     )

#     # If the file does not exist
#     # this is the first release
#     if response.status_code == 404:
#         # We return an empty index, as the platforms will be
#         # appended to the new created package index, it will
#         # not add any modifications to the new created index
#         empty_index = {"packages": [{"platforms": []}]}
#         return empty_index

#     return response.json()


# def get_local_package_index_json():
#     with open(
#         os.path.join(psoc_ino_root_path, "package/package_psoc_index.template.json"),
#         "r",
#     ) as f:
#         data = json.load(f)
#     return data


# def get_platform_data_struct_copy(pkg_index):
#     return copy.deepcopy(pkg_index["packages"][0]["platforms"])


# def set_new_platform_data_fields(platform_data_index, pkg_name, version, repository=""):
#     semver = strip_prefix_from_version(version)
#     platform_data = platform_data_index["packages"][0]["platforms"][0]
#     platform_data["version"] = str(semver)
#     archive_file_name = str(pkg_name) + ".zip"
#     platform_data["archiveFileName"] = archive_file_name

#     if repository == "":
#         platform_data["url"] = "http://localhost:8000/" + str(archive_file_name)
#     else:
#         platform_data["url"] = (
#             "https://github.com/"
#             + str(repository)
#             + "/releases/download/"
#             + str(version)
#             + "/"
#             + str(archive_file_name)
#         )

#     platform_data["checksum"] = "SHA-256:" + str(
#         get_package_sha256(os.path.join(pkg_assets_build_path, archive_file_name))
#     )
#     platform_data["size"] = str(
#         get_package_size(os.path.join(pkg_assets_build_path, archive_file_name))
#     )


# def add_platform_to_package_index(pkg_index, platform):
#     pkg_index["packages"][0]["platforms"].extend(platform)


# def make_package_index_file(pkg_index):
#     pkg_index_json_obj = json.dumps(pkg_index, indent=2)
#     pkg_index_w_path = os.path.join(pkg_assets_build_path, "package_psoc_index.json")
#     with open(pkg_index_w_path, "w") as pkg_file:
#         pkg_file.write(pkg_index_json_obj)


# def build_package_index_json(pkg_name, version, repository, release=True):
#     local_package_index = get_local_package_index_json()
#     set_new_platform_data_fields(local_package_index, pkg_name, version, repository)

#     if release:
#         latest_package_index = get_latest_package_index_json()
#         old_platform_data = get_platform_data_struct_copy(latest_package_index)
#         add_platform_to_package_index(local_package_index, old_platform_data)

#     make_package_index_file(local_package_index)


# def build_release_assets(pkg_name):
#     if os.path.exists(pkg_assets_build_path):
#         shutil.rmtree(pkg_assets_build_path)
#     os.makedirs(pkg_assets_build_path)
#     build_package(pkg_name)
#     zip_package(pkg_name)


#### Parser functions ####
"""
def main_parser_func(args):
    parser.print_help()
"""

# def parser_build_release_assets_func(args):
#     global psoc_ino_root_path
#     global pkg_build_path
#     psoc_ino_root_path = args.root_path
#     pkg_build_path = args.build_path
#     pkg_name = mkdir_package_dir(args.version)
#     build_release_assets(pkg_name)
#     build_package_index_json(pkg_name, args.version, args.repository)


# def parser_build_local_assets_func(args):
#     pkg_name = mkdir_package_dir(version)
#     build_release_assets(pkg_name)
#     build_package_index_json(pkg_name, version, "", False)

"""
class ver_action(argparse.Action):
    def __init__(self, option_strings, dest, **kwargs):
        super().__init__(
            option_strings, dest, nargs=0, default=argparse.SUPPRESS, **kwargs
        )

    def __call__(self, parser, namespace, values, option_string=None):
        print("arduino-core-release version: " + core_release_version)
        parser.exit()


def setup_parser():
    parser = argparse.ArgumentParser(description="arduino-core-release tool")
    parser.add_argument(
        "-v", "--version", action=ver_action, help="arduino-core-release version"
    )
    subparsers = parser.add_subparsers()
    parser.set_defaults(func=main_parser_func)

    parser_release = subparsers.add_parser(
        "build-release", description="Build package release assets"
    )
    parser_release.add_argument("repository", type=str, help="Repository name")
    parser_release.add_argument(
        "version",
        type=str,
        default=version,
        help="Package release version (format: x.y.z)",
    )
    parser_release.add_argument(
        "-r",
        "--root-path",
        type=str,
        default=psoc_ino_root_path,
        help="Path to the arduino-core-psoc root path",
    )
    parser_release.add_argument(
        "-b",
        "--build-path",
        type=str,
        default=pkg_assets_build_path,
        help="Path to build package",
    )
    parser_release.set_defaults(func=parser_build_release_assets_func)

    parser_local = subparsers.add_parser(
        "build-local", description="Build package locally"
    )
    parser_local.set_defaults(func=parser_build_local_assets_func)

    return parser


if __name__ == "__main__":
    parser = setup_parser()
    args = parser.parse_args()
    args.func(args)
"""


def parser():

    def main_parser_func(args):
        parser.print_help()

    # def parser_build_release_assets_func(args):
    #     global xmc_ino_root_path
    #     global pkg_build_path
    #     xmc_ino_root_path = args.root_path
    #     pkg_build_path = args.build_path
    #     build_release_assets(args.version, args.repository)

    class ver_action(argparse.Action):
        def __init__(self, option_strings, dest, **kwargs):
            super().__init__(
                option_strings, dest, nargs=0, default=argparse.SUPPRESS, **kwargs
            )

        def __call__(self, parser, namespace, values, option_string=None):
            print("arduino-core-release version: " + core_release_version)
            parser.exit()

    # General parser
    parser = argparse.ArgumentParser(description="arduino-core-release tool")
    parser.add_argument(
        "-v", "--version", action=ver_action, help="arduino-core-release version"
    )
    subparser = parser.add_subparsers()
    parser.set_defaults(func=main_parser_func)

    # # Pack parser
    # package_parser = subparser.add_parser('pack', description='Pack the core into a package archive')
    # package_parser.add_argument('package-name', type=str, help='Name of the package')
    # package_parser.add_argument('version', type=str, help='Package version (format: x.y.z)')
    # package_parser.add_argument('includes', type=str, nargs='+', help='List of directories and files to include in the package')
    # package_parser.add_argument('-r','--core-root-path', type=str, default=core_root_path, help='Path to the core root directory')
    # package_parser.add_argument('-b','--build-path', type=str, default=build_path, help='Path to build package directory')

    # # Build index parser
    # package_index_parser = subparser.add_parser('build-index', description='Build package index file')
    # package_index_parser.add_argument('url', type=str, help='URL to the package index')
    # package_index_parser.add_argument('archive-package-name', type=str, help='Name of the archive package (with extension)')
    # package_index_parser.add_argument('version', type=str, help='Package version (format: x.y.z)')
    # package_index_parser.add_argument('size', type=int, help='Size of the package archive in bytes')
    # package_index_parser.add_argument('sha256', type=str, help='SHA256 hash of the package archive')
    # package_index_parser.add_argument('-t','--pckg-index-template', type=str, help='Package index template file (including path, relative to the core root directory)')
    # package_index_parser.add_argument('-r','--core-root-path', type=str, default=core_root_path, help='Path to the core root directory')
    # package_index_parser.add_argument('-b','--build-path', type=str, default=build_path, help='Path to build package directory')
    # package_index_parser.add_argument('-k','--keep-old-versions', action='store_true', help='Keep old versions in the package index')

    # Build release package parser
    # core-release.py yml_file version -r root_path -b build_path
    # parser_release = subparser.add_parser('build-release-package', description='Build package release assets')
    # parser_release.add_argument('repository', type=str, help='Repository name')
    # parser_release.add_argument('version', type=str, help='Package release version (format: Vx.y.z)')
    # parser_release.add_argument('-r','--root-path', type=str, default=xmc_ino_root_path, help='Path to the XMC-for-Arduino root path')
    # parser_release.add_argument('-b','--build-path', type=str, default=pkg_assets_build_path, help='Path to build package')
    # parser_release.set_defaults(func=parser_build_release_assets_func)

    # Install local package parser
    # core-release.py install-local version yml_file -r root_path -b build_path

    args = parser.parse_args()
    args.func(args)


# if __name__ == "__main__":
#     parser()


if __name__ == "__main__":

    release_config = ReleaseConfig("release-config.yml")
    # print(rel_config.__doc__)

    # pkg_assets_build_path = os.path.join(psoc_ino_root_path, build_dir_name)

    # package_name = "arduino-core-psoc"
    version = "v0.1.1"
    # includes = ["cores", "tools", os.path.join("extras","mtb-libs"), "examples", "variants",
    #         "boards.txt",
    #         "platform.txt",
    #         "LICENSE.md",
    #         "README.md",
    #         "post_install.sh",
    #     ]

    # Path to this file
    script_dir = os.path.dirname(os.path.abspath(__file__))
    # print(script_dir)
    # The core root path is the parent dir of the script path
    core_root_path = os.path.dirname(script_dir)
    # print(core_root_path)

    build_dir_name = "pkg_build"
    # The build path is the parent dir of the core root path
    build_path = os.path.join(core_root_path, build_dir_name)

    # package = CorePackage(release_config.package_name, version, release_config.includes, core_root_path, build_path)
    # print(release_config.url)
    # print(release_config.index_template)

    # For local installation or to be released?
    # if local:
    # url = "http://localhost:8000/" + + package.get_archive_name()
    # github_url
    # if github url --> prepare complete url including version

    # package_index = PackageIndex("https://here.com", package.get_archive_name(), version, package.get_size(), package.get_sha256(), "package_psoc_index.template.json", core_root_path, build_path)

    # python core-package.py build-release-assets yml_file -r core_root_path
    # python core-package.py install-local -p package_path --local-server server_name
