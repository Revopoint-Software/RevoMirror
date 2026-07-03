# 查找 Inno Setup 编译器

message(STATUS "REVO_MIRROR_VERSION: ${CPACK_PACKAGE_REVO_MIRROR_VERSION}")
message(STATUS "CPACK_PACKAGE_SOURCE_DIR: ${CPACK_PACKAGE_SOURCE_DIR}")


# 查找 Inno Setup 编译器
find_program(INNOSETUP_COMPILER
    NAMES iscc ISCC
    PATHS
        "$ENV{ProgramFiles\(x86\)}/Inno Setup 6"
        "$ENV{ProgramFiles}/Inno Setup 6"
        "$ENV{ProgramFiles\(x86\)}/Inno Setup 5"
        "$ENV{ProgramFiles}/Inno Setup 5"
    DOC "Inno Setup Compiler"
)

if(NOT INNOSETUP_COMPILER)
    message(WARNING "Inno Setup compiler (iscc.exe) not found. Inno Setup packaging will be disabled.")
    return()
endif()

message(STATUS "Found Inno Setup compiler: ${INNOSETUP_COMPILER}")

# set the module path, used for includes
set(INNOSETUP_PACKAGE_PATH "${CPACK_PACKAGE_SOURCE_DIR}/packaging/win/innosetup")

set(INNOSETUP_INSTALL_PACKAGE_PATH "${INNOSETUP_PACKAGE_PATH}/setup/RevoMirror-${CPACK_PACKAGE_REVO_MIRROR_VERSION}-windows-x86_64.exe")

execute_process(
    COMMAND "${INNOSETUP_COMPILER}" 
            "/DPreDefine_AppVersion=${CPACK_PACKAGE_REVO_MIRROR_VERSION}" 
            "mainScript_ui.iss"
    WORKING_DIRECTORY ${INNOSETUP_PACKAGE_PATH}
    RESULT_VARIABLE result
    OUTPUT_VARIABLE output
    ERROR_VARIABLE error
    TIMEOUT 30  # 30秒超时
    OUTPUT_FILE "${INNOSETUP_PACKAGE_PATH}/inno_setup_output.log"
    ERROR_FILE "${INNOSETUP_PACKAGE_PATH}/inno_setup_error.log"
)

if(result EQUAL 0)
    message(STATUS "Inno Setup编译成功 package path: ${INNOSETUP_INSTALL_PACKAGE_PATH}")
else()
    message(FATAL_ERROR "Inno Setup编译失败: ${error}")
endif()




