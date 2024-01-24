mkdir out
clang -march=native -O0 -g -Wall -Werror -Wno-unused-variable -Wno-unused-but-set-variable -Wno-unused-function -Isrc -Iout -o out/code_gen.exe app/code_gen.c
.\out\code_gen.exe > .\out\generated.h
