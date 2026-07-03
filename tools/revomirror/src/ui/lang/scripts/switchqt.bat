@echo off
REM Define the path to the .vscode directory
SET VSCODE_DIR=..\.vscode

echo Select QT version:
echo 1. QT5
echo 2. QT6
set /p choice="Enter the number to switch QT version: "

if "%choice%"=="1" (
    copy "window\settings.qt5.json" "%VSCODE_DIR%\settings.json"
    echo Switched to QT5
) else if "%choice%"=="2" (
    copy "window\settings.qt6.json" "%VSCODE_DIR%\settings.json"
    echo Switched to QT6
) else (
    echo Invalid selection
)