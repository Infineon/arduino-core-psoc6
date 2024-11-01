#!/usr/bin/env bash
cd $(dirname $0)

OS="$(uname -s)"
case "$OS" in
Linux*)
  if [ "$EUID" -ne 0 ]; then
    red="\e[31m"
    reset_color=$(tput sgr0)
    echo -e "${red}"
    echo
    echo "You might need to configure permissions for uploading."
    echo "To do so, run the following command from the terminal:"
    echo "sudo bash \"${PWD}/post_install.sh\""
    echo
    echo -e "${reset_color}"
    exit
  fi

    # The mtb-tools directory is located two levels above the hardware directory
    mtb_tools_path="${PWD}/../../../tools/mtb-tools/"
    mtb_tools_path_abs=$(realpath "${mtb_tools_path}")

    # List directories inside the mtb-tools directory and store them in an array
    # as we don´t know the current installed version
    readarray -t directories < <(ls -d ${mtb_tools_path_abs}/*/)

    # Find the directory starting with "tools_3.*" and 
    # store it in the mtb_tools_path variable
    for dir in "${directories[@]}"; do
        if [[ $dir == *tools_3.* ]]; then
            mtb_tools_ver_path=$dir
            break
        fi
    done

    openocd_path="${mtb_tools_ver_path}/openocd"

    bash ${openocd_path}/udev_rules/install_rules.sh
  ;;
esac