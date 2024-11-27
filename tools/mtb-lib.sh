#!/bin/bash

# This script adds the absolute path to the relative paths
# in variants/board/mtb-libs-xxx.txt files.

# The mtb-libs-xxx.txt are obtained from the mtb-integration and
# added to their respective variant dirs. The contain relative paths
# to different subdirs and files in the {runtime.platform.path}.
# As this path is runtime locally resolved, it can only be added during
# the build process.

# Command
cmd=$1

# Paths
platform_path=$2
build_path=$3

# # Board parameters
board_variant=$4

# # Optional arguments
verbose_flag=$5

function add_linker_script_abs_path {
    # Each board variant has a mtb-libs-xxx.txt file in its variants dir
    linker_flag_file_name="mtb-lib-linker-flags.txt"
    linker_flags_file=${platform_path}/variants/${board_variant}/${linker_flag_file_name}

    # Read the linker flags file
    linker_flags=$(cat ${linker_flags_file})

    if [[ ${verbose_flag} == "-v" ]]; then
        echo "MTB linker flags: ${linker_flags_file}"
    fi

    # Split the content into an array of words
    IFS=' ' read -r -a linker_flags_list <<< "$linker_flags"

    # Look for the elements which includes the linker.ld file
    for link_flag in "${linker_flags_list[@]}"; do
        if [[ ${link_flag} == *".ld"* ]]; then

            # Add the absolute path to the linker script
            linker_script_abs_path=${platform_path}/${link_flag}
            if [[ ${verbose_flag} == "-v" ]]; then
                echo "Linker script abs path: ${linker_script_abs_path}"
            fi

            # Replace the relative path with the absolute path
            linker_flags=$(echo ${linker_flags} | sed "s|${link_flag}|${linker_script_abs_path}|g")

            break
        fi
    done

    if [[ ${verbose_flag} == "-v" ]]; then
        echo "MTB linker flags (abs path): ${linker_flags}"
    fi
    
    # Write the linker flags back to a new file in the build path
    echo "${linker_flags}" > "${build_path}/${linker_flag_file_name}"
}


# function add_inc_dir_bsp_abs_path {

# }

# function add_inc_dirs_mtb_lis_abs_path {

# }

# function add_inc_dir_abs_paths {

# }


function add_abs_paths {
    add_linker_script_abs_path
}

function help {
    echo "help"
}

function print_args {
    echo "-----------------------------------------"
    echo "mtb-lib script arguments"
    echo
    echo "Paths"
    echo "-----"
    echo "platform_path      ${platform_path}"
    echo "build_path         ${build_path}"
    echo
    echo "Board parameters"
    echo "----------------"
    echo "board_variant      ${board_variant}"
    echo
    echo "Optional arguments"
    echo "------------------"
    echo "verbose_flag       ${verbose_flag}"
    echo
    echo "-----------------------------------------"
}

case ${cmd} in
    "add-abs-paths")
        add_abs_paths
        ;;
    "help")
        help
        ;;
   *)
        help
        exit 1
        ;;
esac