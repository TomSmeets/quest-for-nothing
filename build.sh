#!/bin/bash
#
# Copyright (c) 2023 - Tom Smeets <tom@tsmeets.nl>
# build.sh - Build the current project

# Current timings (26-12-2023)
#   O0 -g0 -> 105.0 ms
#   O0 -g  -> 120.5 ms
#   O1 -g  -> 361.5 ms
#   O2 -g  -> 413.2 ms
#   O2 -g0 -> 357.5 ms
set -euo pipefail

# Full build vs quick build
MODE="${1:-s}"

function cc() {
  echo "cc $@"
  clang -O0 -g -Wall -Werror -Wno-unused-variable -Wno-unused-but-set-variable -Wno-unused-function -Isrc "$@"
}

function cc_opt() {
  echo "cc_opt $@"
  clang -O2 -g0 -Wall -Werror -Wno-unused-variable -Wno-unused-but-set-variable -Wno-unused-function -Isrc "$@"
}

if [ $MODE == 's' ]; then
  cc -o out/code_gen app/code_gen.c
fi

./out/code_gen > src/generated.h

if [ $MODE == 's' ]; then
  cc_opt -o out/hot               app/hot.c
  cc_opt -o out/hello             app/hello.c
  cc_opt -o out/quest_for_nothing app/quest_for_nothing.c
  cc_opt -o out/time_cmd          app/time_cmd.c
fi

# Cross compilation
if [ $MODE == 'c' ]; then
  cc -target x86_64-unknown-windows-gnu -o out/hello             app/hello.c
fi

cc -shared -o out/quest_for_nothing.so app/quest_for_nothing.c
touch out/trigger
