# SPDX-FileCopyrightText: 2026 Bartłomiej Ludew <bartekldw>
# SPDX-License-Identifier: MIT

cmake_minimum_required(VERSION 3.16)
project(kwin-border-glow)

set(QT_MIN_VERSION "6.5.0")
set(KF_MIN_VERSION "6.0.0")
set(QT_DEFAULT_MAJOR_VERSION 6)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(ECM REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${CMAKE_MODULE_PATH})

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings)

if(NOT CMAKE_LIBRARY_OUTPUT_DIRECTORY)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
endif()

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets DBus)
find_package(KF6 REQUIRED COMPONENTS CoreAddons Config ConfigWidgets KCMUtils I18n)
find_package(KWin REQUIRED)

add_library(borderglow_alpha MODULE
    src/borderglow.cpp
    src/borderglow_logging.cpp
    src/glowrules.cpp
    include/borderglow.hpp
    include/shaders/resources.qrc
    include/glowrules.hpp
)

target_include_directories(borderglow_alpha PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(borderglow_alpha PRIVATE
    Qt6::Core
    Qt6::Gui
    KF6::CoreAddons
    KF6::ConfigCore
    KF6::ConfigGui
    KWin::kwin
)

install(TARGETS borderglow_alpha DESTINATION ${QT6_INSTALL_PREFIX}/${QT6_INSTALL_PLUGINS}/kwin/effects/plugins)
install(FILES metadata.json DESTINATION ${QT6_INSTALL_PREFIX}/${QT6_INSTALL_PLUGINS}/kwin/effects/plugins RENAME borderglow_alpha.json)

kconfig_add_kcfg_files(borderglow_KCM_SRCS kcm/borderglowsettings.kcfgc)
ki18n_wrap_ui(borderglow_KCM_SRCS kcm/ui/borderglowconfigform.ui)

target_sources(borderglow_alpha PRIVATE ${borderglow_KCM_SRCS})

kcoreaddons_add_plugin(kcm_borderglow
    SOURCES
        src/borderglowconfig.cpp
        include/borderglowconfig.hpp
        ${borderglow_SETTINGS_SRCS}
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

set_target_properties(kcm_borderglow PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/kwin/effects/configs
)
install(TARGETS kcm_borderglow DESTINATION ${QT6_INSTALL_PREFIX}/${QT6_INSTALL_PLUGINS}/kwin/effects/configs)