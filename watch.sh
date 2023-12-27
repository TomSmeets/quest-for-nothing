#!/bin/bash
set -euo pipefail
./build.sh
watchexec -c -i src/generated.h -i out './build.sh'
