# RevoMirror Build Guide

## Overview

This project uses CMake with automatic Qt6, MinGW, and CMake path detection based on the Qt6_DIR environment variable. The build configuration is separated into a `config.cmake` file that is included by the main `CMakeLists.txt`.

## Files

- **`CMakeLists.txt`** - Main CMake configuration file
- **`config.cmake`** - Build configuration with automatic path detection and build setup
- **`build.bat`** - Automated build script (optional, for convenience)
- **`BUILD_README.md`** - This documentation file

## Requirements

- Qt6_DIR environment variable must be set to Qt6 installation root directory
- CMake automatically locates MinGW and other tools from Qt installation

## Usage

### Step 1: Set Environment Variable

Set the Qt6_DIR environment variable to your Qt6 installation root directory:

```batch
# Example for Qt 6.8.3 installation
set Qt6_DIR=D:\Qt\Qt6\6.8.3\

# Or add to system environment variables permanently
```

### Step 2: Build the Project

#### Method 1: Using the automated build script (Recommended)

```batch
# Simply run the build script
build.bat
```

#### Method 2: Manual CMake commands

```batch
# Create and enter build directory (auto-created by config.cmake)
mkdir D:\code\RevoMirror\build\tools\revomirror
cd D:\code\RevoMirror\build\tools\revomirror

# Configure the project with MinGW generator
cmake -G "MinGW Makefiles" D:\code\RevoMirror\tools\revomirror

# Build the project
cmake --build . --config Release --parallel 4
```

#### Method 3: Quick rebuild (after initial configuration)

```batch
# If already configured, just run from build directory:
cd D:\code\RevoMirror\build\tools\revomirror
cmake --build . --config Release --parallel 4
```

## Path Configuration

CMake automatically detects paths based on Qt6_DIR environment variable:

- **Qt Path**: `%Qt6_DIR%/mingw_64`
- **MinGW Path**: Auto-detected from `%Qt6_ROOT%/Tools/mingw*_64` (where Qt6_ROOT is parent of Qt6_DIR)
- **CMake Path**: `%Qt6_ROOT%/Tools/CMake_64`

All path detection and configuration is handled in `config.cmake` which is included by `CMakeLists.txt`.

### Example Directory Structure

```
D:\Qt\Qt6\              # Qt6_ROOT (automatically detected)
├── 6.8.3\              # Qt6_DIR (set by user)
│   └── mingw_64\       # Qt libraries and tools
│       ├── bin\
│       ├── lib\cmake\Qt6\
│       └── ...
└── Tools\              # Shared tools directory
    ├── mingw1310_64\   # MinGW compiler (auto-detected)
    ├── mingw1120_64\   # Or other MinGW versions
    │   └── bin\gcc.exe
    └── CMake_64\       # CMake tool
        └── bin\cmake.exe
```

## System Requirements

- Windows 10/11
- Qt 6.8.3 or higher
- MinGW 13.1.0 or compatible version
- CMake 3.16 or higher

## Build Output

After successful compilation, all files will be located at:
```
D:\code\RevoMirror\build\tools\revomirror\deploy\
```

Contains:
- `RevoMirror.exe` - Main executable
- Qt runtime libraries
- Plugin directories
- Language files
- OpenSSL libraries

## Troubleshooting

### Common Issues

1. **Qt6_DIR not set** - Set the environment variable to Qt6 root directory
2. **Qt not found** - Verify Qt6_DIR points to correct installation
3. **MinGW not found** - Ensure MinGW is installed in Qt Tools directory
4. **CMake not found** - Ensure CMake is installed in Qt Tools directory
5. **Build failed** - Check error messages, may need to clean and rebuild

### Manual Configuration

If automatic detection fails, manually verify your Qt installation structure matches the expected layout.

## Version Information

- Project Version: 1.0.0
- Qt Version: 6.8.3
- Compiler: MinGW 13.1.0
- Build System: CMake 3.16+
