#!/bin/bash
#
# Copyright (c) 2024 - Tom Smeets <tom@tsmeets.nl>
# build.sh - Build the current project
set -euo pipefail

common="-Wall -Werror -Wno-unused-function -Wno-unused-variable -I. -march=native"

debug="-O0 -g"
release="-O2 -g0"

linux=""
windows="-target x86_64-unknown-windows-gnu"

mkdir -p out
clang $common $debug   $linux -o out/hot src/hot.c
clang $common $debug   $linux -o out/void    src/main.c
clang $common $release $linux -o out/void-v1 src/main.c

# clang $common $debug $linux -E src/main.c | wc --bytes | numfmt --to=iec
# clang $common $release $linux -o out/quest-for-nothing src/main.c
# strip -s out/quest-for-nothing
