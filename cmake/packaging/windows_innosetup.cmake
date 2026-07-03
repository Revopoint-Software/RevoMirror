# Inno Setup Packaging
# see options at: https://cmake.org/cmake/help/latest/cpack_gen/innosetup.html

#set(INNOSETUP_APPID "DFF310A3-37A5-3CB3-0283-9CE6E1B7F730")

set(CPACK_PACKAGE_REVO_MIRROR_VERSION "${REVO_MIRROR_VERSION}")
set(CPACK_PACKAGE_SOURCE_DIR "${CMAKE_SOURCE_DIR}")

set(CPACK_POST_BUILD_SCRIPTS "${CMAKE_MODULE_PATH}/post/innosetup.cmake")