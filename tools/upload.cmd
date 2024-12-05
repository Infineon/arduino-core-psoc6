@echo off
setlocal

rem Command
set cmd=%1

rem Paths
set openocd_path=%2
set platform_path=%3
set build_path=%4

rem Board parameters
set board_variant=%5
set board_serial_num=%6
set board_openocd_cfg=%7

rem Project parameters
set project_name=%8

rem Optional arguments
set verbose_flag=%9

rem Command selection
if "%cmd%"=="upload" goto upload
if "%cmd%"=="help" goto help
goto default

:default
    call :help
exit /b 1

:get_openocd_exe
    set openocd_exe=%openocd_path%\bin\openocd.exe
exit /b 0

:get_openocd_search_paths
    rem openocd config files search paths
    rem A board support package might have additional openocd config files in its GeneratedSources path
    set search_paths=-s %openocd_path%\scripts -s %platform_path%\variants\%board_variant%\config\GeneratedSource
exit /b 0 

:get_openocd_cmd
    rem openocd program verify reset exit command.
    rem The target controller openocd configuration %board_openocd_cfg% file is provided by the board.
    rem The serial adapter number %board_serial_num% is provided to ensure the right board is flashed in case of multiple KitProg3s connected.      

    rem The build path requires requires double backslashes for openocd to work
    set build_path_double_slashes=%build_path:\=\\%

    set openocd_cmd=-c "source [find interface/kitprog3.cfg]; adapter serial %board_serial_num%; source [find target/%board_openocd_cfg%]; psoc6 allow_efuse_program off; psoc6 sflash_restrictions 1; program %build_path_double_slashes%\\%project_name%.hex verify reset exit;"
exit /b 0

:get_openocd_log
    rem For quiet mode, the output is stored in an openocd.log file in the build path
    set openocd_log=-l %build_path_double_slashes%\\%project_name%.openocd.log
    if "%verbose_flag%"=="-v" (
        set openocd_log=
    )
exit /b 0 

:get_upload_pattern
    call :get_openocd_exe
    call :get_openocd_search_paths
    call :get_openocd_cmd
    call :get_openocd_log
    set upload_pattern=%openocd_exe% %search_paths% %openocd_cmd% %openocd_log%
exit /b 0

:print_upload_pattern
    echo -----------------------------------------
    echo Openocd sketch upload pattern:
    echo.
    echo %upload_pattern%
    echo.
    echo -----------------------------------------
exit /b 0

:parse_upload_err_code
    set error_code=%1
    rem if error code is 0, the upload was successful
    if "%error_code%"=="0" (
        exit /b 0
    )

    rem Try to provide a more helpful error message
    rem by inspecting the openocd log file
    set openocd_log=%build_path%\%project_name%.openocd.log

    rem Search for the error message in the openocd log file
    for /f "tokens=*" %%a in ('findstr "Error:" "%openocd_log%"') do set error=%%a

    rem Known error messages
    set cmsis_dap_device_not_found=Error: unable to find a matching CMSIS-DAP device

    rem Display user friendly message for known errors
    rem Otherwise, display the error message
    if "%error%"=="%cmsis_dap_device_not_found%" (
        set error_message=Are you sure the board is connected?
    ) else (
        set error_message=%error%
    )

    rem Error message
    rem No colouring added as not supported in Windows cmd
    echo %error_message%

exit /b %error_code%

:upload
    call :get_upload_pattern
    rem This will hide the openocd banner,
    rem which is even printed when using log file output
    set "hide_output=>nul"

    if "%verbose_flag%"=="-v" (
        call :print_args
        call :print_upload_pattern
        set hide_output=
    )

    %upload_pattern% %hide_output%
    call :parse_upload_err_code %errorlevel%
exit /b %errorlevel%

:help
    echo Usage:
    echo.
    echo   upload.cmd upload ^<openocd_path^> ^<platform_path^> ^<build_path^> ^<board_variant^> ^<board_serial_num^> ^<board_openocd_cfg^> ^<project_name^> [-v]
    echo   upload.cmd help
    echo.
    echo Positional arguments for 'upload' command:
    echo.
    echo   openocd_path          Path to the openocd directory
    echo   platform_path         Path to the platform directory
    echo   build_path            Path to the build directory
    echo.
    echo   board_variant         Board variant (OPN name)
    echo   board_serial_num      Serial adapter number (KitProg3)
    echo   board_openocd_cfg     Board target controller openocd configuration file
    echo.
    echo   project_name          Sketch .hex file name
    echo.
    echo Optional arguments:
    echo.
    echo   -v                    Verbose mode. Default is quiet mode which saves output in log file.
exit /b 0 

:print_args
    echo -----------------------------------------
    echo Upload script arguments
    echo.
    echo Paths
    echo -----
    echo openocd_path       %openocd_path%
    echo platform_path      %platform_path%
    echo build_path         %build_path%
    echo.
    echo Board parameters
    echo ----------------
    echo board_variant      %board_variant%
    echo board_serial_num   %board_serial_num%
    echo board_openocd_cfg  %board_openocd_cfg%
    echo.
    echo Project parameters
    echo ------------------
    echo project_name       %project_name%
    echo.
    echo Optional arguments
    echo ------------------
    echo verbose_flag       %verbose_flag%
    echo.
    echo -----------------------------------------
exit /b 0 