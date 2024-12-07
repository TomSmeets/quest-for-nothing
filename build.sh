#!/bin/bash
#
# Copyright (c) 2024 - Tom Smeets <tom@tsmeets.nl>
# build.sh - Build the current project
mkdir -p out && clang -o out/hot src/hot.c && exec ./out/hot "${@}"
