import argparse
import copy
import hashlib
import json
import os
import re
import requests
import shutil

version = "0.0.0"
psoc_ino_root_path = os.path.relpath(os.getcwd())
build_dir_name = "pkg_build"
pkg_assets_build_path = os.path.join(psoc_ino_root_path, build_dir_name)

package_name = "arduino-core-psoc"


def strip_prefix_from_version(version):
    """Strips 'v' or 'V' prefix from version."""
    return re.sub(r"[vV]", "", version)


def mkdir_package_dir(version):
    semver = strip_prefix_from_version(version)
    pkg_name = package_name + "-" + semver
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


def build_package(pkg_name):
    pkg_build_path = os.path.join(pkg_assets_build_path, pkg_name)
    dirs_to_copy = ["cores", "tools", "extras/mtb-integration", "examples", "variants"]
    files_to_copy = [
        "boards.txt",
        "platform.txt",
        "LICENSE.md",
        "README.md",
        "post_install.sh",
        "post_install.bat",
    ]
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


def get_latest_package_index_json():
    response = requests.get(
        "https://github.com/Infineon/arduino-core-psoc/releases/latest/download/package_psoc_index.json"
    )

    # If the file does not exist
    # this is the first release
    if response.status_code == 404:
        # We return an empty index, as the platforms will be
        # appended to the new created package index, it will
        # not add any modifications to the new created index
        empty_index = {"packages": [{"platforms": []}]}
        return empty_index

    return response.json()


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
        platform_data["url"] = "http://localhost:8000/" + str(archive_file_name)
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


def build_release_assets(pkg_name):
    if os.path.exists(pkg_assets_build_path):
        shutil.rmtree(pkg_assets_build_path)
    os.makedirs(pkg_assets_build_path)
    build_package(pkg_name)
    zip_package(pkg_name)


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

    def __call__(self, parser, namespace, values, option_string=None):
        print("psoc-release version: " + version)
        parser.exit()


def setup_parser():
    parser = argparse.ArgumentParser(description="psoc-release tool")
    parser.add_argument(
        "-v", "--version", action=ver_action, help="psoc-release version"
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
        help="Package release version (format: Vx.y.z)",
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
