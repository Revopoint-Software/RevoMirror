@echo off
:: RevoMirror Auto Build Script
:: Automatically creates build directory and uses MinGW generator
:: Usage: build.bat [version]
:: Example: build.bat 1.2.3

:: Parse command line arguments
set "REVO_MIRROR_VERSION="
if not "%~1"=="" (
    set "REVO_MIRROR_VERSION=%~1"
    echo Setting version to: %REVO_MIRROR_VERSION%
) else (
    echo Using default version from config.cmake
)

echo ========================================
echo    RevoMirror Auto Build Script
echo ========================================
echo.

:: Check if Qt6_DIR is set
if "%Qt6_DIR%"=="" (
    echo Error: Qt6_DIR environment variable is not set!
    echo Please set Qt6_DIR to your Qt6 installation root directory.
    echo Example: set Qt6_DIR=D:\Qt\Qt6\6.8.3\
    pause
    exit /b 1
)

:: Set paths
set "PROJECT_ROOT=%~dp0"
set "BUILD_DIR=%PROJECT_ROOT%..\..\build\tools\revomirror"

:: Normalize Qt6_DIR path (remove trailing backslash if present)
set "Qt6_DIR_NORM=%Qt6_DIR%"
if "%Qt6_DIR_NORM:~-1%"=="\" set "Qt6_DIR_NORM=%Qt6_DIR_NORM:~0,-1%"

:: Extract Qt6 root directory
for %%i in ("%Qt6_DIR_NORM%") do set "QT6_ROOT=%%~dpi"

:: Set Qt tool paths
set "QT_PATH=%Qt6_DIR_NORM%\mingw_64"
set "MINGW_PATH=%QT6_ROOT%Tools\mingw1310_64"
set "CMAKE_PATH=%QT6_ROOT%Tools\CMake_64"

:: Auto-detect MinGW version if mingw1310_64 doesn't exist
if not exist "%MINGW_PATH%\bin\gcc.exe" (
    for /d %%d in ("%QT6_ROOT%Tools\mingw*_64") do (
        if exist "%%d\bin\gcc.exe" (
            set "MINGW_PATH=%%d"
            goto :mingw_found
        )
    )
)
:mingw_found

:: Set PATH environment variable
echo Setting up Qt build environment...
set "PATH=%QT_PATH%\bin;%MINGW_PATH%\bin;%CMAKE_PATH%\bin;%PATH%"

:: Verify tools are available
echo Verifying build tools...
where cmake >nul 2>&1
if %errorlevel% neq 0 (
    echo Error: CMake not found! Please check Qt installation.
    pause
    exit /b 1
)

where gcc >nul 2>&1
if %errorlevel% neq 0 (
    echo Error: MinGW GCC not found! Please check Qt installation.
    pause
    exit /b 1
)

echo Found Qt at: %QT_PATH%
echo Found MinGW at: %MINGW_PATH%
echo Found CMake at: %CMAKE_PATH%
echo.

:: Create build directory if it doesn't exist
if not exist "%BUILD_DIR%" (
    echo Creating build directory: %BUILD_DIR%
    mkdir "%BUILD_DIR%"
)

:: Enter build directory
:: cd /d "%BUILD_DIR%"

:: Configure project with MinGW generator
echo Configuring project...
if not "%REVO_MIRROR_VERSION%"=="" (
    echo Passing version %REVO_MIRROR_VERSION% to CMake...
    cmake -B "%BUILD_DIR%" -G "MinGW Makefiles" -DREVO_MIRROR_VERSION="%REVO_MIRROR_VERSION%" "%PROJECT_ROOT%"
) else (
    cmake -B "%BUILD_DIR%" -G "MinGW Makefiles" "%PROJECT_ROOT%"
)

if %errorlevel% neq 0 (
    echo Error: Configuration failed!
    pause
    exit /b 1
)

:: Build project
echo.
echo Building project...
cmake --build "%BUILD_DIR%" --config Release --parallel 4

if %errorlevel% neq 0 (
    echo Error: Build failed!
    pause
    exit /b 1
)

echo.
echo ========================================
echo           Build Completed!
echo ========================================
echo.
echo Build directory: %BUILD_DIR%
echo Deploy directory: %BUILD_DIR%\deploy
echo.
if not "%REVO_MIRROR_VERSION%"=="" (
    echo Built with version: %REVO_MIRROR_VERSION%
) else (
    echo Built with default version from config.cmake
)
echo.
echo Usage: build.bat [version]
echo Example: build.bat 1.2.3

pause
