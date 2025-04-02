@echo off

cd /d %~dp0
echo %cd%

REM Function to set up git submodules
:git_submodule_setup
git submodule init
if ERRORLEVEL 1 exit /b %ERRORLEVEL%
git submodule update --recursive
if ERRORLEVEL 1 exit /b %ERRORLEVEL%


REM Function to set up Arduino core API
:core_api_setup
echo core_api_setup
set "cores_psoc6_dir=%cd%\..\cores\psoc6"
set "core_api_submodule_dir=%cd%\..\extras\arduino-core-api"

xcopy /e /i /y "%core_api_submodule_dir%\api" "%cores_psoc6_dir%\api"

exit /b 0
