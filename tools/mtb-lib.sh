#!/bin/bash

# Command
cmd=$1

# Paths
mtb_tools_path=$2
platform_path=$3
build_path=$4

# Board parameters
board_variant=$5
board_version=$6

# Optional arguments
verbose_flag=$7

function is_bsp_added {
    # The linker.ld file assuming that the rest of the BSP files are present. 
    # It could be the TARGET_APP_xxx folder only, but this way we also ensure its sources are present.
    flash_ld_path=${platform_path}/extras/mtb-integration/bsps/TARGET_APP_${board_variant}/COMPONENT_CM4/TOOLCHAIN_GCC_ARM/linker.ld

    added=false

    if [ -f "${flash_ld_path}" ]; then
        added=true
    fi

    echo ${added}
}

function update_bsp_deps {
    # This function will be used to set the dependencies version in the bsps deps/ folder.

    # Why we need this function? 
    # --------------------------------
    # The ModusTooolbox sets a fix version for the bsp repo sources, but the version of the dependencies
    # are not fixed. They are handled by the manifest system instead.
    # As we are not keeping these bsps under version control, but we retrieve them when initializing
    # the project, we will always get the latest version of the bsp dependencies (according to the manifest configuration).
    # We don´t want libraries to be updated without our knowledge, so we will set the dependencies version with this function.

    json_file_default_name="bsp-deps.json"
    json_file="${platform_path}/variants/${board_variant}/${json_file_default_name}"
    bsp_deps_dir="${platform_path}/extras/mtb-integration/bsps/TARGET_APP_${board_variant}/deps"

    if [ ! -f "${json_file}" ]; then
        echo "JSON file not found: ${json_file}"
        exit 1
    fi

    # Read the JSON file line by line
    while IFS= read -r line; do
        # Extract the asset-name
        if [[ $line =~ \"asset-name\":\ \"([^\"]+)\" ]]; then
            asset_name="${BASH_REMATCH[1]}"
        fi

        # Extract the locked-commit
        if [[ $line =~ \"locked-commit\":\ \"([^\"]+)\" ]]; then
            locked_commit="${BASH_REMATCH[1]}"
            
            # Print the extracted values if -v flag is passed
            if [ "${verbose_flag}" == "-v" ]; then
                echo "Asset Name: $asset_name"
                echo "Locked Commit: $locked_commit"
                echo "-------------------------"
            fi

            # Iterate over each file in the BSP_DEPS_DIR directory
            for file in "$bsp_deps_dir"/*; do
                if [ -f "${file}" ]; then
                    # Get the file name without the extension
                    filename=$(basename -- "${file}")
                    filename="${filename%.*}"

                    # Check if the file name matches the asset name
                    if [ "${filename}" == "${asset_name}" ]; then

                        if [ "${verbose_flag}" == "-v" ]; then
                            echo "Match found: ${file} matches asset name ${asset_name}"
                        fi
                        
                        # Read the content of the .mtbx file
                        content=$(cat "${file}")
                        
                        # Replace the version pattern with the locked-commit value
                        updated_content=$(echo "${content}" | sed -E "s/release-v[0-9]+\.[0-9]+\.[0-9]+/${locked_commit}/g")
                        
                        # Write the updated content back to the .mtbx file
                        echo "${updated_content}" > "${file}"
                        
                        echo "Updated ${file} with locked commit ${locked_commit}"
                    else
                        if [ "${verbose_flag}" == "-v" ]; then
                            echo "No match: ${file} does not match asset name ${asset_name}"
                        fi
                    fi
                fi
            done
        fi
    done < "${json_file}"
}

function add_bsp {
    if [[ ${verbose_flag} == "-v" ]]; then  
        echo "Adding BSP for ${board_variant} version ${board_version}"
    fi
    ${mtb_tools_path}/library-manager/library-manager-cli --project ${platform_path}/extras/mtb-integration --add-bsp-name ${board_variant} --add-bsp-version ${board_version}
}

function get_bsp_deps {
    if [[ ${verbose_flag} == "-v" ]]; then 
        echo "Getting BSP dependencies for ${board_variant} version ${board_version}"
    fi
    cd ${platform_path}/extras/mtb-integration && make getlibs BOARD=${board_variant} CY_TOOLS_PATHS=${mtb_tools_path}
}

function build_bsp {
    if [[ ${verbose_flag} == "-v" ]]; then 
        echo "Building BSP for ${board_variant} version ${board_version}"
    fi
    cd ${platform_path}/extras/mtb-integration && make build BOARD=${board_variant} CY_TOOLS_PATHS=${mtb_tools_path}
}

function get_ccxx_build_flags {
    # This function extracts the compiler flags from the cycompiler file
    # resulting from the mtb-lib build process
    cycompiler_file=${platform_path}/extras/mtb-integration/build/APP_${board_variant}/Debug/.cycompiler

    # Read the content of the cycompiler_file
    build_cmd=$(<"${cycompiler_file}")

    # Split the content into an array of words
    IFS=' ' read -r -a build_cmd_list <<< "${build_cmd}"

    # Find the start index of the flags (after -c)
    local start_idx=0
    for i in "${!build_cmd_list[@]}"; do
        if [ "${build_cmd_list[$i]}" == "-c" ]; then
            start_idx=$((i + 1))
            break
        fi
    done

    # Find the end index of the flags (before the first response file starting with @)
    end_idx=${#build_cmd_list[@]}
    for i in "${!build_cmd_list[@]}"; do
        if [[ "${build_cmd_list[$i]}" == @* ]]; then
            end_idx=$i
            break
        fi
    done

    # Extract the flags
    ccxx_flags=("${build_cmd_list[@]:$start_idx:$((end_idx - start_idx))}")

    # Join the flags into a single string
    joined_flags=$(IFS=' '; echo "${ccxx_flags[*]}")

    # Write the flags to a file in the build directory
    echo "${joined_flags}" > "${build_path}/mtb-lib-cxx-flags.txt"
}

function get_ld_linker_flags {
    cylinker_file=${platform_path}/extras/mtb-integration/build/APP_${board_variant}/Debug/.cylinker

    # Read the content of the cylinker_file
    link_cmd=$(<"$cylinker_file")

    # Split the content into an array of words
    IFS=' ' read -r -a link_cmd_list <<< "$link_cmd"

    # Find the start index of the flags (after arm-none-eabi-g++)
    start_idx=0
    for i in "${!link_cmd_list[@]}"; do
        if [[ "${link_cmd_list[$i]}" == *"arm-none-eabi-g++" ]]; then
            start_idx=$((i + 1))
            break
        fi
    done

    # Find the end index of the flags (after the -T linker script argument)
    end_idx=${#link_cmd_list[@]}
    for i in "${!link_cmd_list[@]}"; do
        if [[ "${link_cmd_list[$i]}" == -T* ]]; then
            end_idx=$((i + 1))
            break
        fi
    done

    # Set the path of the linker script
    linker_script_param_index=$((end_idx - 1))
    link_cmd_list[$linker_script_param_index]="-T ${platform_path}/extras/mtb-integration/${link_cmd_list[$linker_script_param_index]:2}"

    # Extract the flags
    ld_flags=("${link_cmd_list[@]:$start_idx:$((end_idx - start_idx))}")

    # Join the flags into a single string
    joined_flags=$(IFS=' '; echo "${ld_flags[*]}")

    # Write the flags to a file in the out_path
    echo "${joined_flags}" > "${build_path}/mtb-lib-linker-flags.txt"
}

function get_inc_dirs {
    inc_dirs_file=${platform_path}/extras/mtb-integration/build/APP_${board_variant}/Debug/inclist.rsp
    mtb_libs_path=${platform_path}/extras/mtb-integration

    # Read the content of the inc_dirs_file
    inc_list=$(<"${inc_dirs_file}")

    # Split the content into an array of words
    IFS=' ' read -r -a inc_list_list <<< "${inc_list}"

    # Add the extras/mtb-integration path to the include directories
    inc_list_with_updated_path=()
    for inc_dir in "${inc_list_list[@]}"; do
        inc_list_with_updated_path+=("${inc_dir/-I/-I${mtb_libs_path}/}")
    done

    # If windows path, replace backslashes with forward slashes
    for i in "${!inc_list_with_updated_path[@]}"; do
        inc_list_with_updated_path[$i]="${inc_list_with_updated_path[$i]//\\//}"
    done

    # Join the list into a single string
    local joined_inc_dirs=$(IFS=' '; echo "${inc_list_with_updated_path[*]}")

    # Write the include directories to a file in the out_path
    echo "${joined_inc_dirs}" > "${build_path}/mtb-lib-inc-dirs.txt"
}

function get_build_flags {
    # All the building flags, linking flags, and include directories are retrieved
    # to be used to compile the Arduino core sources and the user application sources
    get_ccxx_build_flags
    get_ld_linker_flags
    get_inc_dirs
}

function build {
    if [[ ${verbose_flag} == "-v" ]]; then
        print_args
    fi

    bsp_added=$(is_bsp_added)
    # Add BSP if not added and all its dependencies
    # The BSP is already added in the new mtb-integration submodule.
    # We need to get the dependencies and build the BSP.
    # if [[ ${bsp_added} == "false" ]]; then
    #     add_bsp
    #     update_bsp_deps
    # else
    #     if [[ ${verbose_flag} == "-v" ]]; then
    #         echo "Board support package for ${board_variant} already added"
    #     fi
    # fi
  
    get_bsp_deps
    build_bsp

    get_build_flags

    exit 0
}

function help {
    echo "Usage: "
    echo 
    echo "  bash mtb-lib.sh build <mtb_tools_path> <platform_path> <build_path> <board_variant> <board_version> [-v]"
    echo "  bash mtb-lib.sh help"
    echo
    echo "Positional arguments for 'build' command:"
    echo 
    echo "  mtb_tools_path        Path to the MTB tools directory"
    echo "  platform_path         Path to the platform directory"
    echo "  build_path            Path to the build directory"
    echo
    echo "  board_variant         Board variant (OPN name)"
    echo "  board_version         Board version"
    echo
    echo "Optional arguments:"
    echo
    echo "  -v                    Verbose mode. Default is quiet mode which save output in log file."
}

function print_args {
    echo "-----------------------------------------"
    echo "mtb-lib script arguments"
    echo
    echo "Paths"
    echo "-----"
    echo "mtb_tools_path     ${mtb_tools_path}"
    echo "platform_path      ${platform_path}"
    echo "build_path         ${build_path}"
    echo
    echo "Board parameters"
    echo "----------------"
    echo "board_variant      ${board_variant}"
    echo "board_version      ${board_version}"
    echo
    echo "Optional arguments"
    echo "------------------"
    echo "verbose_flag       ${verbose_flag}"
    echo
    echo "-----------------------------------------"
}

function clean {
    # To be used during development. Not linked to any platform
    # pattern or command.
    echo "Cleaning BSP for ${board_variant} version ${board_version}"
    rm -rf ${platform_path}/extras/mtb-integration/bsps/TARGET_APP_${board_variant}
    rm -rf ${platform_path}/extras/mtb-integration/libs
    rm -rf ${platform_path}/extras/mtb-integration/build
    rm -rf ${platform_path}/extras/mtb_shared  
    rm ${platform_path}/extras/mtb-integration/deps/assetlocks.json 
}

case ${cmd} in
    "build")
        build
        ;;
    "clean")
        clean
        ;;
    "help")
        help
        ;;
   *)
        help
        exit 1
        ;;
esac