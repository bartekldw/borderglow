#!/usr/bin/env bash

# kwin_borders KWin plugin
# SPDX-FileCopyrightText: 2026 Bartłomiej Ludew <bartekldw>
# SPDX-License-Identifier: GPL-3.0-or-later

# Build script for the borderglow KWin plugin.
# Checks for required build tools, runs CMake configure, build, and install

set -e

readonly RED="\e[38;2;239;68;68m"
readonly GREEN="\e[38;2;34;197;94m"
readonly BOLD="\e[1m"
readonly RESET="\e[0m"

check_dep() {
    local bin="$1" pkg="$2"
    if ! command -v "$bin" >/dev/null; then
        missing_deps["$bin"]="$pkg"
    else    
        echo "Detected ${bin}"
    fi
}

echo "Checking dependencies for borderglow build system..."

declare -A missing_deps

check_dep cmake cmake
check_dep g++ gcc
check_dep qmake6 qt6-tools

if [ "${#missing_deps[@]}" -gt 0 ]; then
    declare -A pkg_pacman=([cmake]="cmake" [g++]="gcc" [qmake6]="qt6-tools")
    declare -A pkg_apt=([cmake]="cmake" [g++]="g++" [qmake6]="qt6-tools-dev")
    declare -A pkg_dnf=([cmake]="cmake" [g++]="gcc-c++" [qmake6]="qt6-qttools-devel")

    if command -v pacman >/dev/null; then
        install_cmd="sudo pacman -S"
        declare -n pkg_map=pkg_pacman
    elif command -v apt >/dev/null; then
        install_cmd="sudo apt install"
        declare -n pkg_map=pkg_apt
    elif command -v dnf >/dev/null; then
        install_cmd="sudo dnf install"
        declare -n pkg_map=pkg_dnf
    else
        install_cmd=""
    fi

    echo -e "${RED}ERROR:${RESET} Missing dependencies:"
    pkgs=()
    for bin in "${!missing_deps[@]}"; do
        echo "  - $bin"
        [ -n "$install_cmd" ] && pkgs+=("${pkg_map[$bin]}")
    done

    if [ -n "$install_cmd" ]; then
        echo "Install with:"
        echo "$install_cmd ${pkgs[*]}"
    else
        echo "Could not detect your package manager. Please install the packages listed above manually"
    fi

    exit 1
fi

echo "Base dependencies detected successfully"
echo "Configuring..."

LOG=$(mktemp)
if ! cmake -B build -S . -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_EXPORT_COMPILE_COMMANDS=ON > "$LOG" 2>&1; then
    echo -e "${RED}ERROR:${RESET} CMake configuration failed:"
    cat "$LOG"
    rm "$LOG"
    exit 1
fi
rm "$LOG"

echo "Building..."
cmake --build build -j"$(nproc)"

echo "Installing (requires sudo)..."
sudo cmake --install build

echo -e "${GREEN}${BOLD}SUCCESS:${RESET} Successfully built and installed borderglow. Restart KWin to apply: kwin_wayland --replace & disown or reboot"