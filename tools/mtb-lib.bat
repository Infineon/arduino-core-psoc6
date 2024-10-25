@echo off
setlocal enabledelayedexpansion

REM Get the arguments passed in the command line

REM Command
set cmd=%1

REM Paths
set mtb_tools_path=%2
set platform_path=%3
set build_path=%4

REM Board parameters 
set board_variant=%5
set board_version=%6

REM "Optional arguments"
set verbose_flag=%7

REM Replace backslashes with forward slashes
REM for openocd to work properly
set "mtb_tools_path=!mtb_tools_path:\=/!"
set "platform_path=!platform_path:\=/!"
set "build_path=!build_path:\=/!"

%mtb_tools_path%/modus-shell/bin/bash -i -l -c "bash %platform_path%/tools/mtb-lib.sh %upload_cmd% %mtb_tools_path% %platform_path% %build_path% %board_variant% %board_version% %verbose_flag%"