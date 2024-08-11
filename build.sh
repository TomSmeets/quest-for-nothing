#!/bin/bash
#
# Copyright (c) 2024 - Tom Smeets <tom@tsmeets.nl>
# build.sh - Build the current project
set -euo pipefail

common="-Wall -Werror -Wno-unused-function -Wno-unused-variable -Wno-unused-but-set-variable -I. -march=native"

debug="-O0 -g"
release="-O2 -g0"

linux=""
windows="-target x86_64-unknown-windows-gnu"

mkdir -p out
clang $common $debug  $linux   -o out/hot src/hot.c
clang $common $debug  $linux   -o out/quest-for-nothing src/main.c
clang $common $debug  $windows -o out/quest-for-nothing.exe src/main.c

if [ ! -f out/SDL2.dll ]; then
  pushd out
  curl -L 'https://github.com/libsdl-org/SDL/releases/download/release-2.30.6/SDL2-2.30.6-win32-x64.zip' > SDL2.zip
  unzip SDL2.zip SDL2.dll
  rm SDL2.zip
  popd
fi

# clang $common $debug $linux -E src/main.c | wc --bytes | numfmt --to=iec
# clang $common $release $linux -o out/quest-for-nothing src/main.c
# strip -s out/quest-for-nothing
