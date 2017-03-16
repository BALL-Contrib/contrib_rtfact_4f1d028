@echo off
REM Start script for initbuilder.sh on Windows

echo Going to clone cmake-builder.git...

set PATH=bootstrap;%PATH%
call initbuilder.vbs
if %ERRORLEVEL%==0 (
    echo Finished succesfully. 
) else (
    echo Cloning failed.
)
