#!/usr/bin/env bash
cd $(dirname $0)

OS="$(uname -s)"
case "$OS" in
Linux*)
  if [ "$EUID" -ne 0 ]; then
    echo
    echo "You might need to configure permissions for uploading."
    echo "To do so, run the following command from the terminal:"
    echo "sudo bash \"${PWD}/post_install.sh\""
    echo
    exit
  fi

    # The openocd directory is located two levels above the hardware directory
    openocd_path="${PWD}/../../../tools/openocd/"
    openocd_path_abs=$(realpath "${openocd_path}")

    # List directories inside the openocd directory and store them in an array
    # as we don´t know the current installed version
    readarray -t directories < <(ls -d ${openocd_path_abs}/*/)

    # Get the directory inside the openocd directory.
    # We assume only one version of openocd is installed. Thus, one directory.
    openocd_ver_path=${directories[0]}

    bash ${openocd_ver_path}/udev_rules/install_rules.sh
  ;;
esac