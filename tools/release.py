import argparse
import copy
import hashlib
import json
import os
import re
import requests
import shutil

version = "0.1.0"

psoc_ino_root_path = os.path.relpath(os.getcwd())
build_dir_name = "pkg_build"
pkg_assets_build_path = os.path.join(psoc_ino_root_path, build_dir_name)


# Utility Functions
def strip_prefix_from_version(version):
    return re.sub(r"[vV]", "", version)


def mkdir_package_dir(version):
    semver = strip_prefix_from_version(version)
    pkg_name = "PSOC_IFX_" + semver
    pkg_build_path = os.path.join(pkg_assets_build_path, pkg_name)
    if os.path.exists(pkg_build_path):
        shutil.rmtree(pkg_build_path)
    os.makedirs(pkg_build_path)
    return pkg_name


def get_package_size(pkg):
    return os.path.getsize(pkg)


def get_package_sha256(pkg):
    with open(pkg, "rb") as f:
        bytes = f.read()
        hash = hashlib.sha256(bytes).hexdigest()
    return hash


# Package Building Functions
def build_package(pkg_name):
    pkg_build_path = os.path.join(pkg_assets_build_path, pkg_name)
    dirs_to_copy = ["cores", "tools", "mtb-libs", "examples"]
    files_to_copy = ["boards.txt", "platform.txt", "LICENSE.md", "README.md"]

    for dir in dirs_to_copy:
        shutil.copytree(
            os.path.join(psoc_ino_root_path, dir), os.path.join(pkg_build_path, dir)
        )

    for file in files_to_copy:
        shutil.copyfile(
            os.path.join(psoc_ino_root_path, file), os.path.join(pkg_build_path, file)
        )


def zip_package(pkg_name):
    pkg_build_path = os.path.join(pkg_assets_build_path, pkg_name)
    shutil.make_archive(pkg_build_path, "zip", pkg_assets_build_path, pkg_name)


# Package Index Management Functions
def get_latest_package_index_json():
    return requests.get(
        "https://github.com/Infineon/arduino-core-psoc/releases/download/0.1.0/package_psoc_index.json"
    ).json()


def get_local_package_index_json():
    with open(
        os.path.join(psoc_ino_root_path, "package/package_psoc_index.template.json"),
        "r",
    ) as f:
        data = json.load(f)
    return data


def get_platform_data_struct_copy(pkg_index):
    return copy.deepcopy(pkg_index["packages"][0]["platforms"])


def set_new_platform_data_fields(platform_data_index, pkg_name, version, repository=""):
    semver = strip_prefix_from_version(version)
    platform_data = platform_data_index["packages"][0]["platforms"][0]
    platform_data["version"] = str(semver)
    archive_file_name = str(pkg_name) + ".zip"
    platform_data["archiveFileName"] = archive_file_name

    if repository == "":
        platform_data["url"] = (
            os.path.join(os.getcwd(), build_dir_name) + "/" + str(archive_file_name)
        )
    else:
        platform_data["url"] = (
            "https://github.com/"
            + str(repository)
            + "/releases/download/"
            + str(version)
            + "/"
            + str(archive_file_name)
        )

    platform_data["checksum"] = "SHA-256:" + str(
        get_package_sha256(os.path.join(pkg_assets_build_path, archive_file_name))
    )
    platform_data["size"] = str(
        get_package_size(os.path.join(pkg_assets_build_path, archive_file_name))
    )


def add_platform_to_package_index(pkg_index, platform):
    pkg_index["packages"][0]["platforms"].extend(platform)


def make_package_index_file(pkg_index):
    pkg_index_json_obj = json.dumps(pkg_index, indent=2)
    pkg_index_w_path = os.path.join(pkg_assets_build_path, "package_psoc_index.json")
    with open(pkg_index_w_path, "w") as pkg_file:
        pkg_file.write(pkg_index_json_obj)


def build_package_index_json(pkg_name, version, repository, release=True):
    local_package_index = get_local_package_index_json()
    set_new_platform_data_fields(local_package_index, pkg_name, version, repository)

    if release:
        latest_package_index = get_latest_package_index_json()
        old_platform_data = get_platform_data_struct_copy(latest_package_index)
        add_platform_to_package_index(local_package_index, old_platform_data)

    make_package_index_file(local_package_index)


# Release Asset Management
def build_release_assets(pkg_name):
    if os.path.exists(pkg_assets_build_path):
        shutil.rmtree(pkg_assets_build_path)
    os.makedirs(pkg_assets_build_path)
    build_package(pkg_name)
    zip_package(pkg_name)


# Argument Parsing Functions
def main_parser_func(args):
    parser.print_help()


def parser_build_release_assets_func(args):
    global psoc_ino_root_path
    global pkg_build_path
    psoc_ino_root_path = args.root_path
    pkg_build_path = args.build_path
    pkg_name = mkdir_package_dir(args.version)
    build_release_assets(pkg_name)
    build_package_index_json(pkg_name, args.version, args.repository)


def parser_build_local_assets_func(args):
    pkg_name = mkdir_package_dir(version)
    build_release_assets(pkg_name)
    build_package_index_json(pkg_name, version, "", False)


class ver_action(argparse.Action):
    def __init__(self, option_strings, dest, **kwargs):
        super().__init__(
            option_strings, dest, nargs=0, default=argparse.SUPPRESS, **kwargs
        )

    def __call__(self, parser, namespace, values, option_string, **kwargs):
        print("psoc-release version: " + version)
        parser.exit()


def parser():
    # General parser
    parser = argparse.ArgumentParser(description="psoc-release tool")
    parser.add_argument(
        "-v", "--version", action=ver_action, help="psoc-release version"
    )
    subparser = parser.add_subparsers()
    parser.set_defaults(func=main_parser_func)

    # Release parser
    parser_release = subparser.add_parser(
        "build-release", description="Build package release assets"
    )
    parser_release.add_argument("repository", type=str, help="Repository name")
    parser_release.add_argument(
        "version", type=str, help="Package release version (format: Vx.y.z)"
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

    # Local parser
    parser_local = subparser.add_parser(
        "build-local", description="Build package locally"
    )
    parser_local.set_defaults(func=parser_build_local_assets_func)

    args = parser.parse_args()
    args.func(args)


if __name__ == "__main__":
    parser()
