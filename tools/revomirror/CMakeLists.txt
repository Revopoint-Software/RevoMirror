# revomirror/CMakeLists.txt

cmake_minimum_required(VERSION 3.16)

# Include build configuration
# window下指定使用MinGW64编译工具链，而不是MSVC
if(WIN32)
    include(${CMAKE_CURRENT_SOURCE_DIR}/config.cmake)
endif()

MESSAGE("Revo Mirror build!")

# 设置默认值（如果命令行没有传递）
if(NOT DEFINED REVO_MIRROR_VERSION)
    set(REVO_MIRROR_VERSION "1.0.0.28")
endif()

project(RevoMirror VERSION ${REVO_MIRROR_VERSION})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
#set(QT_NO_ENTRYPOINT ON)

# 查找 Qt
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Network)
# 如果使用 Qt5
#find_package(Qt5 REQUIRED COMPONENTS Core Widgets Network)

# 源文件
set(REVOMIRROR_SOURCES
    src/main.cpp
    src/util/singleapplication.h
    src/util/singleapplication.cpp
    src/util/revoutils.h
    src/util/revoutils.cpp
    src/common.h
    src/util/simplelogger.h
    src/util/configmanager.h
    src/util/configmanager.cpp
    src/ui/lang/language.h
    src/ui/lang/languagemgr.h
    src/ui/lang/languagemgr.cpp
    src/ui/mainwindow.cpp
    src/ui/mainwindow.h
    src/ui/loadingdialog.h
    src/ui/loadingdialog.cpp
    src/ui/gradientline.h
    src/ui/gradientline.cpp
    src/ui/customtitlebar.h
    src/ui/customtitlebar.cpp
    src/ui/mirrorrunning.h
    src/ui/mirrorrunning.cpp
    src/ui/mirrorstopped.h
    src/ui/mirrorstopped.cpp
    src/ui/mirrorpair.h
    src/ui/mirrorpair.cpp
    src/ui/dialogcustomtitlebar.h
    src/ui/dialogcustomtitlebar.cpp
    src/ui/moredialog.h
    src/ui/moredialog.cpp
    src/ui/messagedialog.h
    src/ui/messagedialog.cpp
    src/ui/versiondialog.h
    src/ui/versiondialog.cpp
    src/ui/revodialog.h
    src/ui/revodialog.cpp
    src/ui/circlewaitingwidget.h
    src/ui/circlewaitingwidget.cpp
    src/comm/httprequest.h
    src/comm/httprequest.cpp
    src/bl/versionmanager.h
    src/bl/versionmanager.cpp
    src/bl/httpmanager.h
    src/bl/httpmanager.cpp
)

set(REVOMIRROR_RESOURCES
    resources.qrc
)

# Windows 下添加资源文件
if(WIN32)
    set(APP_ICON_RESOURCE_WINDOWS "${CMAKE_CURRENT_SOURCE_DIR}/revomirror.rc")
    list(APPEND REVOMIRROR_RESOURCES ${APP_ICON_RESOURCE_WINDOWS})
endif()

#set(CMAKE_OSX_DEPLOYMENT_TARGET "" )

# 创建可执行文件
qt_add_executable(${PROJECT_NAME}
    WIN32 MACOSX_BUNDLE
    ${REVOMIRROR_SOURCES}
    ${REVOMIRROR_RESOURCES}
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# 链接 Qt 库
target_link_libraries(${PROJECT_NAME}
    PRIVATE
    Qt::Core
    Qt::Widgets
    Qt::Network
)


# 通过编译器定义传递版本信息
target_compile_definitions(RevoMirror PRIVATE
    PROJECT_VERSION="${PROJECT_VERSION}"
    PROJECT_VERSION_MAJOR=${PROJECT_VERSION_MAJOR}
    PROJECT_VERSION_MINOR=${PROJECT_VERSION_MINOR}
    PROJECT_VERSION_PATCH=${PROJECT_VERSION_PATCH}
    PROJECT_VERSION_BUILD=${PROJECT_VERSION_TWEAK}
)


# Windows 特定设置
if(WIN32)
    set_target_properties(RevoMirror PROPERTIES
        WIN32_EXECUTABLE TRUE
    )

    # 查找 windeployqt
    find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS ${Qt6_DIR}/../../../bin)

    message(STATUS "WINDEPLOYQT_EXECUTABLE: ${WINDEPLOYQT_EXECUTABLE}")

    if(WINDEPLOYQT_EXECUTABLE)
        # 根据构建类型设置参数
        if(CMAKE_BUILD_TYPE STREQUAL "Debug")
            set(DEPLOY_MODE "--debug")
        else()
            set(DEPLOY_MODE "--release")
        endif()

        # 使用 windeployqt 部署 Qt 依赖
        add_custom_command(TARGET RevoMirror POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/deploy"
            COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/deploy/lang"
            COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:RevoMirror>" "${CMAKE_BINARY_DIR}/deploy/"
            COMMAND ${WINDEPLOYQT_EXECUTABLE}
                --dir "${CMAKE_BINARY_DIR}/deploy"
                ${DEPLOY_MODE}
                --no-translations
                --no-system-d3d-compiler
                --no-opengl-sw
                "${CMAKE_BINARY_DIR}/RevoMirror.exe"
            COMMENT "Deploying Qt libraries (${CMAKE_BUILD_TYPE})"

            COMMAND ${CMAKE_COMMAND} -E copy_directory
                "${CMAKE_SOURCE_DIR}/lib"
                "${CMAKE_BINARY_DIR}/deploy"
            COMMENT "copy OpenSSL lib to deploy dir"

            COMMAND ${CMAKE_COMMAND} -E copy_directory
                "${CMAKE_SOURCE_DIR}/src/ui/lang/trans"
                "${CMAKE_BINARY_DIR}/deploy/lang"
            COMMENT "copy language files to deploy dir"
        )
    endif()
endif()


if(APPLE)
    # 绑定模板并传递变量
    configure_file(
        ${CMAKE_SOURCE_DIR}/Info.plist.in
        ${CMAKE_CURRENT_BINARY_DIR}/Info.plist
        @ONLY  # 替换模板中的 @变量@ 为 CMake 变量值
    )

    set_target_properties(${PROJECT_NAME} PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_BINARY_DIR}/Info.plist
    )


    # 查找部署工具
    find_program(MACDEPLOYQT_EXECUTABLE macdeployqt
        HINTS
            ${Qt6_DIR}/../../../bin
            /opt/homebrew/bin
            /usr/local/bin
    )

    message(STATUS "MACDEPLOYQT_EXECUTABLE: ${MACDEPLOYQT_EXECUTABLE}")

    find_program(CODESIGN_EXECUTABLE codesign)

    if(MACDEPLOYQT_EXECUTABLE)
        # 第一步：基本部署
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${MACDEPLOYQT_EXECUTABLE}
                $<TARGET_BUNDLE_DIR:${PROJECT_NAME}>
                -appstore-compliant
                -verbose=2
            COMMENT "Step 1: Deploying Qt frameworks and plugins"
            VERBATIM
        )

        # add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        #     COMMAND ${CMAKE_SOURCE_DIR}/fix_plugins.sh $<TARGET_FILE_DIR:${PROJECT_NAME}>/../..
        #     COMMENT "Step 2: Converting plugins to ARM64"
        #     VERBATIM
        # )

        # 拷贝翻译文件到.app资源路径下
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
                "${CMAKE_SOURCE_DIR}/src/ui/lang/trans"
                "$<TARGET_BUNDLE_DIR:${PROJECT_NAME}>/Contents/Resources/lang"
            COMMENT "copy language files to deploy dir"
        )
        # 拷贝图标文件到.app资源路径下
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy
                "${CMAKE_SOURCE_DIR}/resources/AppIcon.icns"
                "$<TARGET_BUNDLE_DIR:${PROJECT_NAME}>/Contents/Resources/"
            COMMENT "copy AppIcon"
        )

        # 代码签名（使用临时签名）
        # if(CODESIGN_EXECUTABLE)
        #     add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        #         COMMAND ${CODESIGN_EXECUTABLE}
        #             --force
        #             --deep
        #             --verify
        #             --verbose
        #             --sign "-"
        #             $<TARGET_BUNDLE_DIR:${PROJECT_NAME}>
        #         COMMENT "Code signing with ad-hoc signature"
        #         VERBATIM
        #     )
        # endif()

        # 拷贝生成的.app文件到build/package/目录下
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
                "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.app"
                "${CMAKE_SOURCE_DIR}/../../build/package/${PROJECT_NAME}.app"
            COMMENT "copy language files to deploy dir"
        )
    endif()
endif()
