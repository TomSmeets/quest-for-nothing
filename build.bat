mkdir out
clang -march=native -Wall -Werror -Wno-unused-variable -Wno-unused-but-set-variable -Wno-unused-function -Isrc -Iout -o out/code_gen.exe app/code_gen.c
