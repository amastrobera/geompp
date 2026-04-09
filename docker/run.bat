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

REM Resolve absolute path to the repo root (one level up from docker\)
for %%I in ("%~dp0..") do set local_vol=%%~fI

REM Check if the image parameter is provided and valid
if "%image%"=="" (
    echo Error: The -image parameter is required.
    goto usage
)
if /I "%image%"=="Windows" (
    goto run_windows
) else if /I "%image%"=="Linux" (
    goto run_linux
) else (
    echo Error: Invalid value for -image. Accepted values are 'Windows' or 'Linux'.
    goto usage
)


:run_windows
echo ^> Running Windows container
echo ^>   shared volume: %local_vol% -^> C:\Users\developer\workspace
REM No --entrypoint override: Dockerfile CMD sets up the VS x64 environment via VsDevCmd.bat
docker run ^
    --rm ^
    --name geompp-win ^
    --interactive ^
    --tty ^
    --volume "%local_vol%:C:\Users\developer\workspace" ^
    geompp-win:latest
exit /B 0


:run_linux
echo ^> Running Linux container
echo ^>   shared volume: %local_vol% -^> /home/developer/workspace/geompp
docker run ^
    --rm ^
    --name geompp-lin ^
    --publish 3232:3232 ^
    --interactive ^
    --tty ^
    --env DISPLAY=%DISPLAY% ^
    --volume /tmp/.X11-unix:/tmp/.X11-unix ^
    --entrypoint /bin/bash ^
    --volume "%local_vol%:/home/developer/workspace/geompp" ^
    geompp-lin:latest
exit /B 0


:usage
echo Usage: %0 -image ^<Windows^|Linux^>
exit /B 1
