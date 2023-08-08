# This file is part of FlowDrop Qt.
#
# For license and copyright information please follow this link:
# https://github.com/noseam-env/flowdrop-qt/blob/master/LEGAL

cmake_minimum_required(VERSION 3.22)

include(cmake/osconfig.cmake NO_POLICY_SCOPE)

include(cmake/version.cmake)
app_parse_version(version)

set(project_langs C CXX)
if (APPLE)
    set(project_langs C CXX OBJC OBJCXX)
endif ()

project(flowdrop-qt
        LANGUAGES ${project_langs}
        VERSION ${app_version_cmake})

set(CMAKE_CXX_STANDARD 17)

if (MSVC)
    option(BUILD_FOR_MT "" ON)
    set(BUILD_FOR_MT ON)
    add_compile_options(
            $<$<CONFIG:>:/MT> #---------|
            $<$<CONFIG:Debug>:/MTd> #---|-- Statically link the runtime libraries
            $<$<CONFIG:Release>:/MT> #--|
    )
endif ()

set(product_company_name "Michael Neonov")
set(product_company_copyright "Michael Neonov (C) Copyright ${CURRENT_YEAR}")
set(product_name "FlowDrop Qt")
set(product_internal_name "flowdrop-qt")
set(product_original_filename "flowdrop-qt.exe")
set(product_file_description ${product_name})
set(product_comments ${product_name})

# Qt
set(QT_MIN_VERSION "6.5.0")
find_package(Qt6 ${QT_MIN_VERSION} REQUIRED COMPONENTS
        Core
        Gui
        Widgets
        Svg
        Network)
set(FWQ_LINK_LIBS ${FWQ_LINK_LIBS}
        Qt::Core
        Qt::Gui
        Qt::Widgets
        Qt::Svg
        Qt::Network)
if (OS_WINDOWS)
    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(FWQ_LINK_LIBS ${FWQ_LINK_LIBS} ${Qt6_DIR}/../../../plugins/platforms/qwindowsd.lib)
    else ()
        set(FWQ_LINK_LIBS ${FWQ_LINK_LIBS} ${Qt6_DIR}/../../../plugins/platforms/qwindows.lib)
    endif ()
endif ()

# Libraries
add_subdirectory(ThirdParty/GSL)
add_subdirectory(ThirdParty/libflowdrop)
add_subdirectory(ThirdParty/libknotdeviceinfo)
set(FWQ_LINK_LIBS ${FWQ_LINK_LIBS}
        Microsoft.GSL::GSL
        libflowdrop_static
        knotdeviceinfo_static
        nlohmann_json::nlohmann_json)

# Sources
set(FWQ_SOURCES ${FWQ_SOURCES}
        SourceFiles/platform/platform_notifications.h
        SourceFiles/platform/platform_share_target.h
        SourceFiles/platform/platform_tray.h
        SourceFiles/views/receivers_window.cpp
        SourceFiles/views/receivers_window.h
        SourceFiles/views/settings_window.cpp
        SourceFiles/views/settings_window.h
        SourceFiles/application.cpp
        SourceFiles/application.h
        SourceFiles/base_util.cpp
        SourceFiles/base_util.h
        SourceFiles/console_fix.cpp
        SourceFiles/console_fix.h
        SourceFiles/file_lock.h
        SourceFiles/icon_util.cpp
        SourceFiles/icon_util.h
        SourceFiles/launcher.cpp
        SourceFiles/launcher.h
        SourceFiles/qtmaterialcircularprogress.cpp
        SourceFiles/qtmaterialcircularprogress.h
        SourceFiles/qtmaterialcircularprogress_internal.cpp
        SourceFiles/qtmaterialcircularprogress_internal.h
        SourceFiles/qtmaterialcircularprogress_p.h
        SourceFiles/resources.cpp
        SourceFiles/resources.h
        SourceFiles/settings.cpp
        SourceFiles/settings.h
        SourceFiles/single_instance.cpp
        SourceFiles/single_instance.h
        SourceFiles/style.cpp
        SourceFiles/style.h
        SourceFiles/ui_util.cpp
        SourceFiles/ui_util.h)

if (OS_LINUX)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(LIBNOTIFY REQUIRED libnotify)
    set(FWQ_INCLUDE ${FWQ_INCLUDE} ${LIBNOTIFY_INCLUDE_DIRS})
    set(FWQ_LINK_LIBS ${FWQ_LINK_LIBS} ${LIBNOTIFY_LIBRARIES})
    set(FWQ_SOURCES ${FWQ_SOURCES}
            SourceFiles/platform/linux/notifications_linux.cpp
            SourceFiles/platform/linux/share_target_linux.cpp
            SourceFiles/platform/linux/tray_linux.cpp
            SourceFiles/platform/main.cpp
            SourceFiles/file_lock_posix.cpp)
elseif (OS_MACOS)
    set(bundle_identifier "me.nelonn.FlowDropQt")
    set(MACOSX_BUNDLE_NAME "FlowDrop")
    set(MACOSX_BUNDLE_GUI_IDENTIFIER ${bundle_identifier})
    set(MACOSX_BUNDLE_GUI_COPYRIGHT ${product_company_copyright})
    set(MACOSX_BUNDLE_INFO_STRING "FlowDrop wirelessly send files between devices")
    set(MACOSX_BUNDLE_LONG_VERSION_STRING ${app_version_string})
    set(MACOSX_BUNDLE_SHORT_VERSION_STRING ${app_version_string_small})
    set(MACOSX_BUNDLE_ICON_FILE AppIcon.icns)
    configure_file(
            Resources/Info.plist.in
            ${CMAKE_CURRENT_BINARY_DIR}/Resources/Info.plist)
    set(MACOS_BUNDLE_ICON_FILE "${CMAKE_CURRENT_SOURCE_DIR}/Resources/AppIcon.icns")
    set_source_files_properties(${MACOS_BUNDLE_ICON_FILE} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
    set(FWQ_SOURCES ${FWQ_SOURCES}
            ${MACOS_BUNDLE_ICON_FILE}
            SourceFiles/platform/mac/notifications_mac.mm
            SourceFiles/platform/mac/share_target_mac.cpp
            SourceFiles/platform/mac/tray_mac.mm
            SourceFiles/platform/main.cpp
            SourceFiles/file_lock_posix.cpp)
elseif (OS_WINDOWS)
    # WinToast
    add_definitions(-DNDEBUG)
    option(WINTOASTLIB_BUILD_EXAMPLES "" OFF)
    add_subdirectory(ThirdParty/WinToast)

    configure_file(
            Resources/winrc/app.rc.in
            ${CMAKE_CURRENT_BINARY_DIR}/winrc/version.rc
            @ONLY)
    set(FWQ_LINK_LIBS ${FWQ_LINK_LIBS} WinToast rstrtmgr.dll user32.lib runtimeobject.lib ComCtl32.lib Advapi32.lib)
    set(FWQ_SOURCES ${FWQ_SOURCES}
            Resources/winrc/app.manifest
            ${CMAKE_CURRENT_BINARY_DIR}/winrc/version.rc
            SourceFiles/platform/win/notifications_win.cpp
            SourceFiles/platform/win/share_target_win.cpp
            SourceFiles/platform/win/tray_win.cpp
            SourceFiles/platform/main_win.cpp
            SourceFiles/file_lock_win.cpp
    )
endif ()

qt6_add_resources(FWQ_RESOURCES Resources/resources.qrc)
set(FWQ_SOURCES ${FWQ_SOURCES} ${FWQ_RESOURCES})

add_executable(flowdrop-qt WIN32 MACOSX_BUNDLE ${FWQ_SOURCES})

set_target_properties(flowdrop-qt PROPERTIES AUTORCC TRUE)
set_target_properties(flowdrop-qt PROPERTIES AUTOMOC TRUE)

target_include_directories(flowdrop-qt PRIVATE ${FWQ_INCLUDE})
target_link_libraries(flowdrop-qt PRIVATE ${FWQ_LINK_LIBS})

target_include_directories(flowdrop-qt PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/SourceFiles")

if (OS_MACOS)
    set_target_properties(flowdrop-qt PROPERTIES MACOSX_BUNDLE TRUE)
    set_target_properties(flowdrop-qt PROPERTIES
            OUTPUT_NAME "FlowDrop"
            MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_BINARY_DIR}/Resources/Info.plist
            XCODE_ATTRIBUTE_ENABLE_HARDENED_RUNTIME YES
            XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY libc++
    )
elseif (OS_WINDOWS)
    target_compile_definitions(flowdrop-qt PRIVATE _WINDOWS NOMINMAX)
endif ()
