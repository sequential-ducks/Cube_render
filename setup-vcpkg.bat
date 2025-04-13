@echo off
setlocal enabledelayedexpansion

:: Set error handling
set "EXIT_ON_ERROR=if errorlevel 1 exit /b %errorlevel%"

set VCPKG_DIR=external\vcpkg

:: Ensure git submodule is added and initialized
echo Cloning vcpkg as a submodule...
git submodule add https://github.com/microsoft/vcpkg.git %VCPKG_DIR% 2>nul || ver>nul
%EXIT_ON_ERROR%
git submodule update --init --recursive
%EXIT_ON_ERROR%

:: Navigate to vcpkg directory
cd %VCPKG_DIR%
%EXIT_ON_ERROR%

:: Update vcpkg to latest version
echo Updating vcpkg to latest version...
git checkout master
%EXIT_ON_ERROR%
git pull origin master
%EXIT_ON_ERROR%

:: Bootstrap vcpkg
echo Bootstrapping vcpkg...
call bootstrap-vcpkg.bat
%EXIT_ON_ERROR%

:: Install graphics libs
echo Installing glad...
vcpkg install glad:x64-windows
%EXIT_ON_ERROR%

echo vcpkg setup complete!

endlocal