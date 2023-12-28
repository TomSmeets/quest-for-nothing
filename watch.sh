#!/bin/bash
set -euo pipefail
watchexec -c -i src/generated.h -i out './build.sh'
