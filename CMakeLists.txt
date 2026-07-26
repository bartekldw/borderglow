# SPDX-FileCopyrightText: 2026 Bartłomiej Ludew <bartekldw>
# SPDX-License-Identifier: MIT

cmake_minimum_required(VERSION 3.16)
project(kwin-border-glow)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(ECM REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${CMAKE_MODULE_PATH})

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)
find_package(KF6 REQUIRED COMPONENTS CoreAddons Config)
find_package(KWin REQUIRED)

add_library(borderglow_alpha MODULE
    src/borderglow.cpp
    src/borderglow_logging.cpp
    include/borderglow.hpp
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
    KWin::kwin
)

install(TARGETS borderglow_alpha DESTINATION ${QT6_INSTALL_PREFIX}/${QT6_INSTALL_PLUGINS}/kwin/effects/plugins)