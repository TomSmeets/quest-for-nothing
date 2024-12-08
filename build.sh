#!/bin/bash
#
# Copyright (c) 2024 - Tom Smeets <tom@tsmeets.nl>
# build.sh - Build the current project
mkdir -p out
clang -O0 -g -o out/build src/hot.c && exec ./out/build "${@}"
