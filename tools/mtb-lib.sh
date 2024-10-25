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
    flash_ld_path=${platform_path}/mtb-libs/bsps/TARGET_APP_${board_variant}/COMPONENT_CM4/TOOLCHAIN_GCC_ARM/linker.ld

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
    bsp_deps_dir="${platform_path}/mtb-libs/bsps/TARGET_APP_${board_variant}/deps"

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
    ${mtb_tools_path}/library-manager/library-manager-cli --project ${platform_path}/mtb-libs --add-bsp-name ${board_variant} --add-bsp-version ${board_version}
}

function get_bsp_deps {
    if [[ ${verbose_flag} == "-v" ]]; then 
        echo "Getting BSP dependencies for ${board_variant} version ${board_version}"
    fi
    cd ${platform_path}/mtb-libs && make getlibs BOARD=${board_variant} CY_TOOLS_PATHS=${mtb_tools_path}
}

function build_bsp {
    if [[ ${verbose_flag} == "-v" ]]; then 
        echo "Building BSP for ${board_variant} version ${board_version}"
    fi
    cd ${platform_path}/mtb-libs && make build BOARD=${board_variant} CY_TOOLS_PATHS=${mtb_tools_path}
}

function build {
    if [[ ${verbose_flag} == "-v" ]]; then
        print_args
    fi

    bsp_added=$(is_bsp_added)
    # Add BSP if not added and all its dependencies
    if [[ ${bsp_added} == "false" ]]; then
        add_bsp
        update_bsp_deps
        get_bsp_deps
    else
        if [[ ${verbose_flag} == "-v" ]]; then
            echo "Board support package for ${board_variant} already added"
        fi
    fi

    build_bsp

    #get_build_flags

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
    rm -rf ${platform_path}/mtb-libs/bsps/TARGET_APP_${board_variant}
    rm -rf ${platform_path}/mtb-libs/libs
    rm -rf ${platform_path}/mtb-libs/build
    rm -rf ${platform_path}/mtb_shared  
    rm ${platform_path}/mtb-libs/deps/assetlocks.json 
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