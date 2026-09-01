# SPDX-FileCopyrightText: 2026 Bartłomiej Ludew <bartekldw>
# SPDX-License-Identifier: GPL-3.0-or-later

cmake_minimum_required(VERSION 3.16)
project(borderglow VERSION 1.0 LANGUAGES CXX)

set(QT_MIN_VERSION "6.5.0")
set(KWIN_MIN_VERSION "6.7.4")
set(ECM_MIN_VERSION "6.5.0")
set(KF_MIN_VERSION "6.5.0")
set(QT_DEFAULT_MAJOR_VERSION 6)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(ECM ${ECM_MIN_VERSION} REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${CMAKE_MODULE_PATH})

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings)

if(NOT CMAKE_LIBRARY_OUTPUT_DIRECTORY)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
endif()

find_package(Qt6 ${QT_MIN_VERSION} REQUIRED COMPONENTS Core Gui Widgets DBus)
find_package(KF6 ${KF_MIN_VERSION} REQUIRED COMPONENTS CoreAddons Config ConfigWidgets KCMUtils I18n)
find_package(KWin ${KWIN_MIN_VERSION} REQUIRED)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/metadata.json.in
    ${CMAKE_CURRENT_BINARY_DIR}/metadata.json
    @ONLY
)

add_library(borderglow MODULE
    src/borderglow.cpp
    src/borderglow_logging.cpp
    src/glowrules.cpp
    include/borderglow.hpp
    include/glowrules.hpp
    include/shaders/resources.qrc
)

target_include_directories(borderglow PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(borderglow PRIVATE
    Qt6::Core
    Qt6::Gui
    KF6::CoreAddons
    KF6::ConfigCore
    KF6::ConfigGui
    KWin::kwin
)

install(TARGETS borderglow
    DESTINATION ${KDE_INSTALL_PLUGINDIR}/kwin/effects/plugins
)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/metadata.json
    DESTINATION ${KDE_INSTALL_PLUGINDIR}/kwin/effects/plugins
    RENAME borderglow.json
)

kconfig_add_kcfg_files(borderglow_KCM_SRCS kcm/borderglowsettings.kcfgc)
ki18n_wrap_ui(borderglow_KCM_SRCS kcm/ui/borderglowconfigform.ui)

target_sources(borderglow PRIVATE ${borderglow_KCM_SRCS})

kcoreaddons_add_plugin(kcm_borderglow
    SOURCES
        src/borderglowconfig.cpp
        include/borderglowconfig.hpp
        ${borderglow_KCM_SRCS}
    INSTALL_NAMESPACE "kwin/effects/configs"
)

target_include_directories(kcm_borderglow PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(kcm_borderglow PRIVATE
    Qt6::Core
    Qt6::Widgets
    Qt6::DBus
    KF6::CoreAddons
    KF6::ConfigCore
    KF6::ConfigWidgets
    KF6::KCMUtils
    KF6::I18n
)