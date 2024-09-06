CC_WARN=-Wall -Werror -Wno-unused-function -Wno-unused-variable -Wno-unused-but-set-variable -Wno-format

CC_DBG=-O0 -g
CC_REL=-O3 -Xlinker --strip-all

CC_LINUX=-march=native
CC_WINDOWS=-target x86_64-unknown-windows-gnu
CC_WASM=-target wasm32 --no-standard-libraries -Wl,--no-entry -Wl,--export-all -fno-builtin -msimd128

build:
	./build.sh

format:
	clang-format --verbose -i src/*


quest-for-nothing.elf:
	clang $(CC_WARN) $(CC_DBG) $(CC_LINUX) -o out/$@ src/main.c

quest-for-nothing.exe:
	clang $(CC_WARN) $(CC_DBG) $(CC_WINDOWS) -o out/$@ src/main.c
