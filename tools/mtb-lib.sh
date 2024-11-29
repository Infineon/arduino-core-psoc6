#!/bin/bash

# Command
cmd=$1

# Paths
platform_path=$2
build_path=$3

# # Board parameters
board_variant=$4

# # Optional arguments
verbose_flag=$5

function help {
    echo "The mtb-lib.sh script adds the absolute paths to the relative paths"
    echo "in variants/<board>/mtb-lib-xxx.txt files."
    echo "The mtb-lib-xxx.txt files are obtained from mtb-integration and"
    echo "the paths are relative to the {runtime.platform.path} directory."
    echo "This platform path is runtime locally resolved. Thus it can only"
    echo "be added during the build process."
    echo 
    echo "Usage: "
    echo 
    echo "  bash mtb-lib.sh add-abs-paths <platform_path> <build_path> <board_variant> [-v]"
    echo "  bash mtb-lib.sh help"
    echo
    echo "Positional arguments for 'add-abs-paths' command:"
    echo 
    echo "  platform_path         Path to the platform directory"
    echo "  build_path            Path to the build directory"
    echo
    echo "  board_variant         Board variant (OPN name)"
    echo
    echo "Optional arguments:"
    echo
    echo "  -v                    Verbose mode"
}

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

function add_inc_dirs_abs_path {
    # Each board variant has a mtb-libs-inc-dir.txt file in its variants dir
    inc_dirs_file_name="mtb-lib-inc-dirs.txt"
    inc_dirs_file=${platform_path}/variants/${board_variant}/${inc_dirs_file_name}

    # Read the inc dirs file
    inc_dirs=$(cat ${inc_dirs_file})

    if [[ ${verbose_flag} == "-v" ]]; then
        echo "MTB inc dirs: ${inc_dirs_file}"
    fi

    # Split the content into an array of words
    IFS=' ' read -r -a inc_dirs_list <<< "$inc_dirs"

    # For each element in the list, add the absolute path
    for inc_dir in "${inc_dirs_list[@]}"; do
        # Each inc_dir starts with "-I". We insert the platform
        # path between the "-I" and the relative path:
        # -I${platform_path}/relative/path/to/dir
        inc_dir_abs_path=-I${platform_path}/${inc_dir:2}        
        if [[ ${verbose_flag} == "-v" ]]; then
            echo "Inc dir abs path: ${inc_dir_abs_path}"
        fi

        # Replace the relative path with the absolute path
        inc_dirs=$(echo ${inc_dirs} | sed "s|${inc_dir}|${inc_dir_abs_path}|g")
    done

    if [[ ${verbose_flag} == "-v" ]]; then
        echo "MTB inc dirs (abs path): ${inc_dirs}"
    fi

    # Write the inc dirs back to a new file in the build path
    echo "${inc_dirs}" > "${build_path}/${inc_dirs_file_name}"
}

function add_abs_paths {
    add_linker_script_abs_path
    add_inc_dirs_abs_path
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