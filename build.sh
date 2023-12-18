#!/bin/bash
#
# Copyright (c) 2023 - Tom Smeets <tom@tsmeets.nl>
# build.sh - Build the current project
set -euo pipefail

# Full buld vs quick build
MODE="${1:-s}"

if [ $MODE == 's' ]; then
  echo '#pragma once' > src/typedefs.h
  rg -Ior 'typedef $1 $2 $2;' '^(struct|union) +([^ {]+)' src app | sort -u >> src/typedefs.h
fi

function cc() {
  echo "cc $@"
  clang -O0 -ggdb -Wall -Werror -Wno-unused-variable -Wno-unused-but-set-variable -Wno-unused-function -Isrc "$@"
}

if [ $MODE == 's' ]; then
  cc -o out/hot   app/hot.c
  cc -o out/parse app/parse.c
  cc -o out/code_gen app/code_gen.c
fi

# cc -shared -o out/quest_for_nothing.so app/quest_for_nothing.c
cc -shared -o out/hello.so app/hello.c
# cc -o out/hello app/hello.c
touch out/trigger
# clang -O0 -ggdb -Wall -Werror -Wno-unused-variable -Wno-unused-but-set-variable -Wno-unused-function -Isrc -o out/parse src/main_parse.c
# ./out/parse
