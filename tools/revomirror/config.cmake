# =============================================================================
# RevoMirror Build Configuration
# Automatically detects Qt6, MinGW, and CMake paths based on Qt6_DIR environment variable
# =============================================================================

MESSAGE("RevoMirror Build Configuration")
MESSAGE("==============================")

# Set default version if not provided
if(NOT REVO_MIRROR_VERSION)
    set(REVO_MIRROR_VERSION "1.0.0")
endif()

# Check if Qt6_DIR environment variable is set
if(NOT DEFINED ENV{Qt6_DIR} OR "$ENV{Qt6_DIR}" STREQUAL "")
    message(FATAL_ERROR 
        "Qt6_DIR environment variable is not set!\n"
        "Please set Qt6_DIR to your Qt6 installation root directory.\n"
        "Example: set Qt6_DIR=D:\\Qt\\Qt6\\6.8.3\\")
endif()

# Get Qt6_DIR from environment
set(Qt6_DIR_ENV "$ENV{Qt6_DIR}")
message(STATUS "Qt6_DIR from environment: ${Qt6_DIR_ENV}")

# Normalize Qt6_DIR path (remove trailing slash if present)
string(REGEX REPLACE "[\\\\/]+$" "" Qt6_DIR_NORM "${Qt6_DIR_ENV}")

# Extract Qt6 root directory (e.g., D:/Qt/Qt6/6.8.3/ -> D:/Qt/Qt6/)
get_filename_component(QT6_ROOT "${Qt6_DIR_NORM}" DIRECTORY)

# Set Qt installation path
set(QT_INSTALL_PATH "${Qt6_DIR_NORM}/mingw_64")

# Auto-detect MinGW compiler in Tools directory
set(MINGW_INSTALL_PATH "")
file(GLOB MINGW_CANDIDATES "${QT6_ROOT}/Tools/mingw*_64")

foreach(mingw_dir ${MINGW_CANDIDATES})
    if(EXISTS "${mingw_dir}/bin/gcc.exe")
        set(MINGW_INSTALL_PATH "${mingw_dir}")
        break()
    endif()
endforeach()

# Set CMake path
set(CMAKE_INSTALL_PATH "${QT6_ROOT}/Tools/CMake_64")

# Display detected paths
message(STATUS "Qt Path: ${QT_INSTALL_PATH}")
message(STATUS "MinGW Path: ${MINGW_INSTALL_PATH}")
message(STATUS "CMake Path: ${CMAKE_INSTALL_PATH}")
message(STATUS "")

# Verify Qt installation
if(NOT EXISTS "${QT_INSTALL_PATH}/lib/cmake/Qt6")
    message(FATAL_ERROR "Qt6 not found at ${QT_INSTALL_PATH}")
endif()
message(STATUS "Found Qt6 at: ${QT_INSTALL_PATH}")

# Verify MinGW compiler
if("${MINGW_INSTALL_PATH}" STREQUAL "")
    message(FATAL_ERROR "No MinGW compiler found in ${QT6_ROOT}/Tools/")
endif()
if(NOT EXISTS "${MINGW_INSTALL_PATH}/bin/gcc.exe")
    message(FATAL_ERROR "MinGW compiler not found at ${MINGW_INSTALL_PATH}")
endif()
message(STATUS "Found MinGW at: ${MINGW_INSTALL_PATH}")

# Verify CMake (optional, as we're already using CMake)
if(EXISTS "${CMAKE_INSTALL_PATH}/bin/cmake.exe")
    message(STATUS "Found CMake at: ${CMAKE_INSTALL_PATH}")
else()
    message(WARNING "CMake not found at expected location: ${CMAKE_INSTALL_PATH}")
endif()

# Set CMake variables for the build
set(CMAKE_PREFIX_PATH "${QT_INSTALL_PATH}")
set(Qt6_DIR "${QT_INSTALL_PATH}/lib/cmake/Qt6")

# Set compiler paths
set(CMAKE_C_COMPILER "${MINGW_INSTALL_PATH}/bin/gcc.exe")
set(CMAKE_CXX_COMPILER "${MINGW_INSTALL_PATH}/bin/g++.exe")

# Set default generator to MinGW Makefiles if not specified and MinGW is available
if(WIN32 AND NOT CMAKE_GENERATOR AND MINGW_INSTALL_PATH)
    set(CMAKE_GENERATOR "MinGW Makefiles" CACHE STRING "CMake generator" FORCE)
    message(STATUS "Set default generator to MinGW Makefiles")
endif()

# Set build type to Release if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif()

# Auto-create build directory and handle in-source builds
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
    message(STATUS "In-source build detected, setting up out-of-source build...")
    set(AUTO_BUILD_DIR "${CMAKE_SOURCE_DIR}/../../build/tools/revomirror")
    
    # Create build directory automatically
    file(MAKE_DIRECTORY "${AUTO_BUILD_DIR}")
    message(STATUS "Created build directory: ${AUTO_BUILD_DIR}")
    
    message(STATUS "To use out-of-source build, run:")
    message(STATUS "  cd \"${AUTO_BUILD_DIR}\"")
    if(WIN32 AND MINGW_INSTALL_PATH)
        message(STATUS "  cmake -G \"MinGW Makefiles\" \"${CMAKE_SOURCE_DIR}\"")
    else()
        message(STATUS "  cmake \"${CMAKE_SOURCE_DIR}\"")
    endif()
    message(STATUS "  cmake --build . --config Release --parallel 4")
    message(STATUS "")
    message(STATUS "Or simply run from build directory:")
    message(STATUS "  cmake --build . --config Release --parallel 4")
endif()

message(STATUS "Configuration completed successfully!")
message(STATUS "==============================")
message(STATUS "")
