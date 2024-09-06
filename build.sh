#!/bin/bash
#
# Copyright (c) 2024 - Tom Smeets <tom@tsmeets.nl>
# build.sh - Build the current project
set -euo pipefail

COMMON_WARN="-Wall -Werror -Wno-unused-function -Wno-unused-variable -Wno-unused-but-set-variable -Wno-format"

COMMON_DBG="-O1 -g"
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

mkdir -p out

set -x
clang $LINUX_DBG -o out/hot src/hot.c &

clang $LINUX_DBG -o out/main.elf  src/main.c
clang $WIN32_DBG -o out/main.exe  src/main.c
clang $WASM_DBG  -o out/main.wasm src/main.c

clang $LINUX_REL -o out/main-opt.elf  src/main.c
clang $WIN32_REL -o out/main-opt.exe  src/main.c
clang $WASM_REL  -o out/main-opt.wasm src/main.c

cp src/os_wasm.html out/index.html
set +x

wait

if [ ! -f out/SDL2.dll ]; then
  pushd out
  curl -L 'https://github.com/libsdl-org/SDL/releases/download/release-2.30.6/SDL2-2.30.6-win32-x64.zip' > SDL2.zip
  unzip SDL2.zip SDL2.dll
  rm SDL2.zip
  popd
fi
