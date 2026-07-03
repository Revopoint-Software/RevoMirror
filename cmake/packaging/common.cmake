# common packaging

# common cpack options
set(CPACK_PACKAGE_NAME "${REVO_MIRROR_NAME}")
set(CPACK_PACKAGE_VENDOR "Revopoint")
set(CPACK_PACKAGE_CompanyName "Revopoint 3D Technologies Inc.")
set(CPACK_PACKAGE_CompanyURL "https://www.revopoint3d.com/")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
set(CPACK_PACKAGE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/cpack_artifacts)
set(CPACK_PACKAGE_CONTACT "https://app.lizardbyte.dev")
set(CPACK_PACKAGE_DESCRIPTION ${CMAKE_PROJECT_DESCRIPTION})
set(CPACK_PACKAGE_HOMEPAGE_URL ${CMAKE_PROJECT_HOMEPAGE_URL})
set(CPACK_RESOURCE_FILE_LICENSE ${PROJECT_SOURCE_DIR}/LICENSE)
set(CPACK_PACKAGE_ICON ${PROJECT_SOURCE_DIR}/sunshine.png)
# 获取时间戳
string(TIMESTAMP BUILD_TIMESTAMP "%Y%m%d_%H%M%S")
# 组合版本号和时间戳
#set(CPACK_PACKAGE_FILE_NAME "${REVO_MIRROR_NAME}_v${PROJECT_VERSION}_${BUILD_TIMESTAMP}")
#set(CPACK_PACKAGE_FILE_NAME "${REVO_MIRROR_NAME}_v${PROJECT_VERSION}")
set(CPACK_PACKAGE_FILE_NAME "${REVO_MIRROR_NAME}")
set(CPACK_STRIP_FILES YES)

# install common assets
install(DIRECTORY "${SUNSHINE_SOURCE_ASSETS_DIR}/common/assets/"
        DESTINATION "${SUNSHINE_ASSETS_DIR}"
        PATTERN "web" EXCLUDE)
# copy assets to build directory, for running without install
file(GLOB_RECURSE ALL_ASSETS
        RELATIVE "${SUNSHINE_SOURCE_ASSETS_DIR}/common/assets/" "${SUNSHINE_SOURCE_ASSETS_DIR}/common/assets/*")
list(FILTER ALL_ASSETS EXCLUDE REGEX "^web/.*$")  # Filter out the web directory
foreach(asset ${ALL_ASSETS})  # Copy assets to build directory, excluding the web directory
    file(COPY "${SUNSHINE_SOURCE_ASSETS_DIR}/common/assets/${asset}"
            DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/assets")
endforeach()

# install built vite assets
install(DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/assets/web"
        DESTINATION "${SUNSHINE_ASSETS_DIR}")

# platform specific packaging
if(WIN32)
    include(${CMAKE_MODULE_PATH}/packaging/windows.cmake)
elseif(UNIX)
    include(${CMAKE_MODULE_PATH}/packaging/unix.cmake)

    if(APPLE)
        include(${CMAKE_MODULE_PATH}/packaging/macos.cmake)
    else()
        include(${CMAKE_MODULE_PATH}/packaging/linux.cmake)
    endif()
endif()

include(CPack)
