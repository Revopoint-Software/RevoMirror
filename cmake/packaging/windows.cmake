# windows specific packaging
install(TARGETS sunshine RUNTIME DESTINATION ".")

# Hardening: include zlib1.dll (loaded via LoadLibrary() in openssl's libcrypto.a)
install(FILES "${ZLIB}" DESTINATION ".")

# ARM64: include minhook-detours DLL (shared library for ARM64)
if(NOT CMAKE_SYSTEM_PROCESSOR MATCHES "AMD64" AND DEFINED _MINHOOK_DLL)
    install(FILES "${_MINHOOK_DLL}" DESTINATION "." COMPONENT application)
endif()

# copy apps.json to config directory
set(BUILD_CONFIG_DIR "${CMAKE_CURRENT_BINARY_DIR}/config")
set(APPS_JSON_SOURCE "${SUNSHINE_SOURCE_ASSETS_DIR}/windows/assets/apps.json")
set(APPS_JSON_BUILD "${BUILD_CONFIG_DIR}/apps.json")
file(MAKE_DIRECTORY ${BUILD_CONFIG_DIR})
configure_file(${APPS_JSON_SOURCE} ${APPS_JSON_BUILD} COPYONLY)
install(FILES ${APPS_JSON_BUILD}
        DESTINATION config)

# Adding tools
install(TARGETS dxgi-info RUNTIME DESTINATION "tools")
install(TARGETS audio-info RUNTIME DESTINATION "tools")

# Mandatory tools
install(TARGETS sunshinesvc RUNTIME DESTINATION "tools")

install(DIRECTORY "${CMAKE_BINARY_DIR}/tools/revomirror/deploy/" 
        DESTINATION ".")

# Mandatory scripts
install(FILES "${SUNSHINE_SOURCE_ASSETS_DIR}/windows/misc/sunshine-setup.ps1"
        DESTINATION "scripts"
        COMPONENT assets)
install(DIRECTORY "${SUNSHINE_SOURCE_ASSETS_DIR}/windows/misc/service/"
        DESTINATION "scripts")
install(DIRECTORY "${SUNSHINE_SOURCE_ASSETS_DIR}/windows/misc/migration/"
        DESTINATION "scripts")
install(DIRECTORY "${SUNSHINE_SOURCE_ASSETS_DIR}/windows/misc/path/"
        DESTINATION "scripts")

# scripts
install(DIRECTORY "${SUNSHINE_SOURCE_ASSETS_DIR}/windows/misc/firewall/"
        DESTINATION "scripts")

# Sunshine assets
install(DIRECTORY "${SUNSHINE_SOURCE_ASSETS_DIR}/windows/assets/"
        DESTINATION "${SUNSHINE_ASSETS_DIR}")

# copy assets (excluding shaders) to build directory, for running without install
file(COPY "${SUNSHINE_SOURCE_ASSETS_DIR}/windows/assets/"
        DESTINATION "${CMAKE_BINARY_DIR}/assets"
        PATTERN "shaders" EXCLUDE)
# use junction for shaders directory
cmake_path(CONVERT "${SUNSHINE_SOURCE_ASSETS_DIR}/windows/assets/shaders"
        TO_NATIVE_PATH_LIST shaders_in_build_src_native)
cmake_path(CONVERT "${CMAKE_BINARY_DIR}/assets/shaders" TO_NATIVE_PATH_LIST shaders_in_build_dest_native)
execute_process(COMMAND cmd.exe /c mklink /J "${shaders_in_build_dest_native}" "${shaders_in_build_src_native}")

set(CPACK_PACKAGE_ICON "${CMAKE_SOURCE_DIR}\\\\sunshine.ico")

# The name of the directory that will be created in C:/Program files/
set(CPACK_PACKAGE_INSTALL_DIRECTORY "${CPACK_PACKAGE_NAME}")

set(CPACK_COMPONENT_GROUP_THIRDPARTY_DISPLAY_NAME "Third Party")
set(CPACK_COMPONENT_GROUP_THIRDPARTY_DESCRIPTION "Bundled third-party installers and optional components.")
# include specific packaging
include(${CMAKE_MODULE_PATH}/packaging/windows_nsis.cmake)
include(${CMAKE_MODULE_PATH}/packaging/windows_wix.cmake)
include(${CMAKE_MODULE_PATH}/packaging/windows_innosetup.cmake)
