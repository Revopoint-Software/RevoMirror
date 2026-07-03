#!/bin/bash
APP_PATH="$1"
PLUGINS_DIR="$APP_PATH/Contents/PlugIns"

echo "Converting Universal binary plugins to ARM64..."
find "$PLUGINS_DIR" -name "*.dylib" | while read dylib; do
    if lipo "$dylib" -verify_arch x86_64 2>/dev/null; then
        echo "Converting: $dylib"
        lipo "$dylib" -thin arm64 -output "$dylib.arm64"
        mv "$dylib.arm64" "$dylib"
    else
        echo "Already ARM64 only: $dylib"
    fi
done
echo "Plugin conversion completed."
