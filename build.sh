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
MODE="${1:-slow}"

REL="-O2 -g0"
OPT="-O2 -g"
DBG="-O0 -g"

WIN="-target x86_64-unknown-windows-gnu"

function cc() {
    echo "cc $@"
    clang -march=native -Wall -Werror -Wno-unused-variable -Wno-unused-but-set-variable -Wno-unused-function -Wno-format -I. "$@"
}

cc $DBG -o out/code_gen app/code_gen.c
./out/code_gen > out/generated.h

if [ $MODE == "slow" ]; then
  cc $OPT -o out/hot               app/hot.c
  cc $OPT -o out/hello             app/hello.c
  cc $OPT -o out/quest_for_nothing app/quest_for_nothing/main.c
  cc $OPT -o out/time_cmd          app/time_cmd.c
  cc $OPT -o out/todotreesrv       app/todotreesrv.c
  cc $OPT -o out/ledger            app/ledger/main.c
  cc $DBG -o out/test app/test.c
  ./out/test

  # cc $OPT $WIN -o out/hello.exe app/hello.c
  # cc $OPT $WIN -o out/quest_for_nothing.exe app/quest_for_nothing.c
fi

# cc $DBG -shared -o out/quest_for_nothing.so app/quest_for_nothing/main.c
# cc $DBG -o out/todotreesrv app/todotreesrv.c
if [ $MODE == "ledger" ]; then
    cc $DBG -o out/ledger      app/ledger/main.c
    # ./out/ledger -f /tree/now/life/ledger_ing.txt
    ./out/ledger -c /tree/now/life/ing_betaal.csv -m /tree/now/life/mapping_ledger_ing.txt
fi

touch out/trigger
