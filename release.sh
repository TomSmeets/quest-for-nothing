mkdir -p out/release
./build build src/qfn/qfn.c out/release/quest_for_nothing.elf  release linux
./build build src/qfn/qfn.c out/release/quest_for_nothing.exe  release windows
./build build src/qfn/qfn.c out/release/quest_for_nothing.wasm release wasm
