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

REM common parameters for running
set swd=%~dp0%
set local_vol=%swd%..
set image_vol=/home/developer/workspace/geompp

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



REM Run commands
:run_windows
set image_tag=geompp-win
set image_name=geompp-win

echo "> Running Windows"
goto run

:run_linux
set image_tag=geompp-lin
set image_name=geompp-lin

echo "> Running Linux"
goto run


:run
echo "    > tag %image_tag%"
echo "    > shared volume %local_vol%"
docker run ^
    --rm ^
    --name %image_name% ^
    --publish 3232:3232 ^
    --interactive ^
    --tty ^
    --env DISPLAY=$DISPLAY ^
    --volume /tmp/.X11-unix:/tmp/.X11-unix ^
    --entrypoint /bin/bash ^
    --volume %local_vol%:%image_vol% ^
    %image_tag%:latest
exit /B 0

REM #---- notes ------
REM # to run on WSL 
REM   replace
REM    --net host ^
REM   with
REM    --publish 8000:8000 ^

REM # to run interactively
REM    --interactive ^
REM    --tty ^
REM    --entrypoint /bin/bash ^
REM    --volume %local_vol%:%image_vol% ^


REM Function to display usage information
:usage
echo Usage: %0 -image ^<Windows^|Linux^>
exit /B 1

:end
endlocal