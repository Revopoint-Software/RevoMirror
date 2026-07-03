#!/bin/bash
# switch-config.sh

# Define the path to the .vscode directory
VSCODE_DIR="../.vscode"

echo "Select QT version:"
echo "1. QT5"
echo "2. QT6"
read -p "Enter the number to choose the configuration: " choice

case $choice in
  1)
    cp "./mac/settings.qt5.json" "$VSCODE_DIR/settings.json"
    echo "Switched to QT5"
    ;;
  2)
    cp "./mac/settings.qt6.json" "$VSCODE_DIR/settings.json"
    echo "Switched to QT6"
    ;;
  *)
    echo "Invalid selection"
    ;;
esac