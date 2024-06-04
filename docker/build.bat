@echo off
setlocal enabledelayedexpansion


REM Parse the input parameter
set image=
if "%1"=="-image" (
    set image=%2
) else (
    echo Unknown parameter passed: %1
    goto usage
)

REM Get the absolute path to the directory containing the script
REM for %%I in (%~dp0) do set "script_dir=%%~fI"
set local_vol=%~dp0%\

REM Check if the image parameter is provided and valid
if "%image%"=="" (
    echo Error: The -image parameter is required.
    goto usage
)
if /I "%image%"=="Windows" (
    goto build_windows
) else if /I "%image%"=="Linux" (
    goto build_linux
) else (
    echo Error: Invalid value for -image. Accepted values are 'Windows' or 'Linux'.
    goto usage
)



REM Build commands
:build_windows
echo build windows
docker build -t "geompp-win" -f "%local_vol%Dockerfile.win" %local_vol%
goto end

:build_linux
echo build linux
docker build -t "geompp-lin" -f "%local_vol%Dockerfile.lin" %local_vol%
goto end


REM Function to display usage information
:usage
echo Usage: %0 -image ^<Windows^|Linux^>
exit /B 1

:end
endlocal