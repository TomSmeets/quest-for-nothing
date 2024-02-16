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

REL="-O2 -g0"
OPT="-O2 -g"
DBG="-O0 -g"

WIN="-target x86_64-unknown-windows-gnu"

function cc() {
    echo "cc $@"
    clang -march=native -Wall -Werror -Wno-unused-variable -Wno-unused-but-set-variable -Wno-unused-function -Wno-format -Isrc -Iout "$@"
}

if [ $MODE == "s" ]; then
    cc $OPT -o out/code_gen app/code_gen.c
fi

./out/code_gen > out/generated.h

if [ $MODE == "s" ]; then
  cc $OPT -o out/hot               app/hot.c
  cc $OPT -o out/hello             app/hello.c
  cc $OPT -o out/quest_for_nothing app/quest_for_nothing/main.c
  cc $OPT -o out/time_cmd          app/time_cmd.c

  cc $DBG -o out/test app/test.c
  ./out/test

  # cc $OPT $WIN -o out/hello.exe app/hello.c
  # cc $OPT $WIN -o out/quest_for_nothing.exe app/quest_for_nothing.c
fi

cc $DBG -shared -o out/quest_for_nothing.so app/quest_for_nothing/main.c
touch out/trigger
