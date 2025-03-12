#!/bin/bash

# Change dir to the directory where this script is located
cd $(dirname $0)
echo ${PWD}

function git_submodule_setup {
    echo "Setting up Git Submodules..."
    git submodule init
    git submodule update --recursive
}

function core_api_setup {
    echo "Setting up Arduino Core API..."
    cores_psoc6_dir="${PWD}/../cores/psoc6"
    core_api_submodule_dir="${PWD}/../extras/arduino-core-api"

    cp -r ${core_api_submodule_dir}/api ${cores_psoc6_dir}/api
}

function replace_string_h_with_wstring_h {
    # This function replaces String.h with WString.h in Arduino core APIs
    # to avoid the issues with case insensitivity in windows.
    # Issue can be followed here: https://github.com/arduino/ArduinoCore-API/issues/37
    echo "Replacing String_h with WString_h in Arduino core APIs..."

    # Find and rename String.h to WString.h
    find ${cores_psoc6_dir}/api -type f -name 'String.h' -execdir mv {} WString.h \;

    # Find and update include statements in C++ source files
    find ${cores_psoc6_dir}/api -type f \( -name '*.cpp' -o -name '*.h' \) -exec sed -i 's/#include "String.h"/#include "WString.h"/g' {} \;
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
    replace_string_h_with_wstring_h
    # bsps_setup #TODO: Remove this after discussion on how to include MTB sources (symlink or copy)
fi
