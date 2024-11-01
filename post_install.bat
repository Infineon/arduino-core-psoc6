@echo off
setlocal

REM Move to the path of this script
cd /d "%~dp0"

REM Define the path to the mtb-tools dirs
set "mtb_tools_path=%~dp0..\..\..\tools\mtb-tools"

REM Resolve the full path
for %%I in ("%mtb_tools_path%") do set "mtb_tools_path=%%~fI"
%
REM Find the directory starting with "tools_3.*" and 
REM store it in the mtb_tools_path variable
for /d %%I in ("%mtb_tools_path%\tools_3.*") do set "mtb_tools_ver_path=%%~fI"

echo mtb-tools path: %mtb_tools_ver_path%

REM Change to the modus-shell directory
cd /d "%mtb_tools_ver_path%\modus-shell"

call postinstall.bat

endlocal