CC_WARN=-Wall -Werror -Wno-unused-function -Wno-unused-variable -Wno-unused-but-set-variable -Wno-format

CC_DBG=-O0 -g
CC_OPT=-O3 -Xlinker --strip-all

CC_LINUX=-march=native
CC_WINDOWS=-target x86_64-unknown-windows-gnu
CC_WASM=-target wasm32 --no-standard-libraries -Wl,--no-entry -Wl,--export-all -fno-builtin -msimd128

all:: out/main.elf out/main.exe out/main.wasm out/main-opt.elf out/main-opt.exe out/main-opt.wasm

build::
	./build.sh

out:
	mkdir -p out

clean::
	rm -rf out

out/SDL2.dll: out
	curl -L 'https://github.com/libsdl-org/SDL/releases/download/release-2.30.6/SDL2-2.30.6-win32-x64.zip' -o out/SDL2.zip
	unzip -o out/SDL2.zip SDL2.dll -d out

format::
	clang-format --verbose -i src/*

out/hot:
	clang $(CC_WARN) $(CC_DBG) $(CC_LINUX) -o $@ src/hot.c

out/main.elf:: out
	clang $(CC_WARN) $(CC_DBG) $(CC_LINUX) -o $@ src/main.c

out/main.exe:: out out/SDL2.dll
	clang $(CC_WARN) $(CC_DBG) $(CC_WINDOWS) -o $@ src/main.c

out/main.wasm:: out
	clang $(CC_WARN) $(CC_DBG) $(CC_WASM) -o $@ src/main.c

out/main-opt.elf:: out
	clang $(CC_WARN) $(CC_OPT) $(CC_LINUX) -o $@ src/main.c

out/main-opt.exe:: out out/SDL2.dll
	clang $(CC_WARN) $(CC_OPT) $(CC_WINDOWS) -o $@ src/main.c

out/main-opt.wasm:: out
	clang $(CC_WARN) $(CC_OPT) $(CC_WASM) -o $@ src/main.c
