#!/usr/bin/env bash

OS="$(uname -s)"
case "$OS" in
Linux*)
  if [ "$EUID" -ne 0 ]; then

    red="\e[31m"
    reset_color=$(tput sgr0)
    echo -e "${red}"
    if [ -e "${PWD}/post_install.sh" ]; then
      echo
      echo "You might need to configure permissions for uploading."
      echo "To do so, run the following command from the terminal:"
      echo "sudo bash \"${PWD}/post_install.sh\""
      echo
    else
      # Script was executed from another path. It is assumed this will only occur when user is executing script directly.
      # So it is not necessary to provide the command line.
      echo "Please run as root"
    fi
    echo -e "${reset_color}"
    exit
  fi

    # The mtb-tools directory is located two levels above the hardware directory
    mtb_tools_path="${PWD}/../../../tools/mtb-tools/"
    mtb_tools_path=$(realpath "${mtb_tools_path}")

    # List directories inside the mtb-tools directory and store them in an array
    # as we don´t know the current installed version
    readarray -t directories < <(ls -d ${mtb_tools_path}/*/)

    # Use the first directory found. 
    # Usually there is only one directory
    mtb_tools_path=${directories[0]}

    openocd_path="${mtb_tools_path}/openocd"

    bash ${openocd_path}/udev_rules/install_rules.sh
  ;;
esac