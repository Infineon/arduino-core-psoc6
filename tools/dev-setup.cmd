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


REM Function to replace String.h with WString.h in Arduino core APIs
:replace_string_h_with_wstring_h

REM Find and rename String.h to WString.h
for /r "%cores_psoc6_dir%\api" %%f in (String.h) do (
    rename "%%f" WString.h
)

REM Update include statements to reflect the new name
for /r "%cores_psoc6_dir%\api" %%f in (*.h *.cpp) do (
    powershell -Command "(Get-Content -Path '%%f') -replace '#include \"String.h\"', '#include \"WString.h\"' | Set-Content -Path '%%f'"
)

exit /b 0
