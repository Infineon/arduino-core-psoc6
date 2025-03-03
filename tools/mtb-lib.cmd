@echo off
setlocal

rem Command
set "cmd=%1"

rem Paths
set "platform_path=%2"
set "build_path=%3"

rem Board variant
set "board_variant=%4"

rem Optional arguments
set "verbose_flag=%5"

rem Command selection
if "%cmd%"=="add-abs-paths" goto add_abs_paths
if "%cmd%"=="help" goto help
goto default

:default
    call :help
exit /b 1

:add_linker_script_abs_paths
    rem Each board variant has a mtb-libs-linker-flags.txt file in its variants dir
    set "linker_flag_file_name=mtb-lib-linker-flags.txt"
    set "linker_flags_file=%platform_path%\variants\%board_variant%\%linker_flag_file_name%"

    rem Read the linker flags file
    set "linker_flags="
    setlocal enabledelayedexpansion
    for /F "usebackq delims=" %%l in ("%linker_flags_file%") do (
        set "linker_flags=!linker_flags! %%l"
    )

    if "%verbose_flag%" == "-v" (
        echo MTB linker flags: %linker_flags%
    )

    rem Look for the element which includes the linker.ld file
    rem Looping over this string with a simple loop will split the string
    rem into tokens using the "=" as delimiter. This is not what we want.
    rem There we have to use the -f function and the delims option to split.
    :find_ld_file_loop
    for /f "tokens=1,* delims= " %%a in ("!linker_flags!") do (
        rem Save the remaining list of the linker flags in a variable
        set "linker_flags_remaining=%%b"

        rem We need to find the .ld file. Therefore we capture the last 3 characters
        rem of the substring and check if it matches ".ld".
        set "string_end=%%a"
        set "string_end=!string_end:~-3!"
        if "!string_end!" == ".ld" (

            rem Add the absolute path to the linker script
            set "linker_script_abs_path=%platform_path%\%%a"
            if "%verbose_flag%"=="-v" (
                echo Linker script abs path: !linker_script_abs_path!
            )

            rem Change fordward slashes to double backslashes
            set "linker_script_abs_path=!linker_script_abs_path:/=\!"
            set "linker_script_abs_path=!linker_script_abs_path:\=\\!"

            rem Replace the relative path with the absolute path
            rem in the new linker flags with abs path variable
            set "linker_flags_abs_path=!linker_flags_abs_path! !linker_script_abs_path!"
        ) else (
            set "linker_flags_abs_path=!linker_flags_abs_path! %%a"
        )

        rem Check if there are more linker flags to process
        rem and continue the loop if that is the case
        set "linker_flags=!linker_flags_remaining!"
        if "!linker_flags_remaining!" NEQ "" goto find_ld_file_loop
    )

    if "%verbose_flag%"=="-v" (
        echo MTB linker flags with abs path: %linker_flags_abs_path%
    )

    rem Write the linker flags back to a new file in the build path
    echo %linker_flags_abs_path% > "%build_path%\%linker_flag_file_name%"
exit /b 0

:add_inc_dirs_abs_paths
    rem Each board variant has a mtb-libs-inc-dirs.txt file in its variants dir
    set "inc_dirs_file_name=mtb-lib-inc-dirs.txt"
    set "inc_dirs_file=%platform_path%\variants\%board_variant%\%inc_dirs_file_name%"

    rem Read the inc dirs file
    set "inc_dirs="
    setlocal enabledelayedexpansion
    for /F "usebackq delims=" %%l in ("%inc_dirs_file%") do (
        set "inc_dirs=!inc_dirs! %%l"
    )

    if "%verbose_flag%" == "-v" (
        echo MTB inc dirs: %inc_dirs%
    )

    rem For each element in the list, add the absolute path
    for %%a in (!inc_dirs!) do (
        rem Remove the prefix "-I" from the string
        set "dir=%%a"
        set "dir_no_prefix=!dir:~2!"

        rem Replace the forward slashes with backslashes
        set "dir_no_prefix=!dir_no_prefix:/=\!"
        rem Prepend the platform path to the relative path
        set "dir_abs_path=-I%platform_path%\!dir_no_prefix!"
        rem Change backslashes to double backsplashes
        set "dir_abs_path=!dir_abs_path:\=\\!"
        
		if "%verbose_flag%" == "-v" (
			echo MTB inc dirs: !dir_abs_path!
		)
		echo !dir_abs_path! >> "%build_path%\%inc_dirs_file_name%"
    )


exit /b 0

:add_abs_paths
    call :add_linker_script_abs_paths
    call :add_inc_dirs_abs_paths
exit /b 0

:help
    echo The mtb-lib.bat script adds the absolute paths to the relative paths
    echo in variants\<board>\mtb-lib-xxx.txt files.
    echo The mtb-lib-xxx.txt files are obtained from mtb-integration and
    echo the paths are relative to the {runtime.platform.path} directory.
    echo This platform path is runtime locally resolved. Thus it can only
    echo be added during the build process.
    echo.
    echo Usage:
    echo.
    echo   mtb-lib.bat add-abs-paths ^<platform_path^> ^<build_path^> ^<board_variant^> [-v]
    echo   mtb-lib.bat help
    echo.
    echo Positional arguments for 'add-abs-paths' command:
    echo.
    echo   platform_path         Path to the platform directory
    echo   build_path            Path to the build directory
    echo.
    echo   board_variant         Board variant (OPN name)
    echo.
    echo Optional arguments:
    echo.
    echo   -v                    Verbose mode
exit /b 0
