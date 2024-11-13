#!/bin/bash
#
# Copyright (c) 2024 - Tom Smeets <tom@tsmeets.nl>
# build.sh - Build the current project
set -euo pipefail

COMMON_WARN="-Wall -Werror -Wno-unused-function -Wno-unused-variable -Wno-unused-but-set-variable -Wno-format"

COMMON_DBG="-O0 -g"
COMMON_REL="-O3 -Xlinker --strip-all"

COMMON_LINUX="-march=native"
COMMON_WIN32="-target x86_64-unknown-windows-gnu"
COMMON_WASM="-target wasm32 --no-standard-libraries -Wl,--no-entry -Wl,--export-all -fno-builtin -msimd128"

LINUX_DBG="$COMMON_WARN $COMMON_LINUX $COMMON_DBG"
LINUX_REL="$COMMON_WARN $COMMON_LINUX $COMMON_REL"

WIN32_DBG="$COMMON_WARN $COMMON_WIN32 $COMMON_DBG"
WIN32_REL="$COMMON_WARN $COMMON_WIN32 $COMMON_REL"

WASM_DBG="$COMMON_WARN $COMMON_WASM $COMMON_DBG"
WASM_REL="$COMMON_WARN $COMMON_WASM $COMMON_REL"

build_out() {
  mkdir -p out
}

build_clean() {
  rm -rf out
}

build_sdl_dll() {
  if [ ! -f out/SDL2.dll ]; then
    build_out
  	curl -L 'https://github.com/libsdl-org/SDL/releases/download/release-2.30.6/SDL2-2.30.6-win32-x64.zip' -o out/SDL2.zip
  	unzip -o out/SDL2.zip SDL2.dll -d out
  fi
}

build_rel() {
  build_sdl_dll

  mkdir -p out/rel
  clang $LINUX_REL -o out/rel/main.elf  src/main.c &
  clang $WIN32_REL -o out/rel/main.exe  src/main.c &
  clang $WASM_REL  -o out/rel/main.wasm src/main.c &
  cp src/os_wasm.html out/rel/index.html
  cp out/SDL2.dll out/rel
  wait
}

build_deploy() {
  build_rel
  sudo cp -vr out/rel/* /usr/share/caddy/game/
}

build_hot() {
  build_out
  clang $LINUX_DBG -o out/hot src/hot.c
}

build_dbg() {
  build_out
  clang $LINUX_DBG -o out/main.elf  src/main.c
  clang $WIN32_DBG -o out/main.exe  src/main.c
  clang $WASM_DBG  -o out/main.wasm src/main.c
  cp src/os_wasm.html out/index.html
}

# Format Source Code
build_format() {
  clang-format --verbose -i src/*
}

# Build all targets
build_all() {
  build_hot
  build_dbg
  build_rel
}

# Show help message
build_help() {
  declare -F | cut -d '_' -f 2
}

"build_${1:-help}"
