#!/bin/bash

# Change dir to the directory where this script is located
cd $(dirname $0)
echo ${PWD}

function git_submodule_setup {
    git submodule init
    git submodule update --recursive
}

function core_api_setup {
    cores_psoc6_dir="${PWD}/../cores/psoc6"
    core_api_submodule_dir="${PWD}/../extras/arduino-core-api"

    # Create symbolic link (overwrite) to the api/ 
    # folder of ArduinoCore-API submodule.
    # Note: Symlinks might not work without full paths
    # https://stackoverflow.com/questions/8601988/symlinks-not-working-when-link-is-made-in-another-directory
    ln -sf ${core_api_submodule_dir}/api ${cores_psoc6_dir}
}

function bsps_setup {
    bsps_dir="${PWD}/../extras/mtb-integration/bsps"
    variants_dir="${PWD}/../variants"

    # Create symbolic link in the respective variant directory
    # Note: Symlinks might not work without full paths
    # https://stackoverflow.com/questions/8601988/symlinks-not-working-when-link-is-made-in-another-directory   

    for bsp_dir in ${bsps_dir}/*; do
        target_app_board=$(basename ${bsp_dir})
        # Remove the TARGET_APP_ prefix from board name
        board=${target_app_board#TARGET_APP_}
        ln -sf ${bsps_dir}/${target_app_board} ${variants_dir}/${board}/mtb-bsp
    done
}

# Check if a function name is passed as an argument
if [ $# -gt 0 ]; then
    if declare -f "$1" > /dev/null; then
        "$1"
    else
        echo "Function $1 not found"
        exit 1
    fi
else
    git_submodule_setup
    core_api_setup
    # bsps_setup #TODO: Remove this after discussion on how to include MTB sources (symlink or copy)
fi
