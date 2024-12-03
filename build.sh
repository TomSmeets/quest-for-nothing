#!/bin/bash
#
# Copyright (c) 2024 - Tom Smeets <tom@tsmeets.nl>
# build.sh - Build the current project
clang -o out/hot src/hot.c && ./out/hot "${@:-build}"
