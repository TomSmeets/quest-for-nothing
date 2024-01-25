mkdir out
clang -march=native -O0 -g -Wall -Werror -Wno-unused-variable -Wno-unused-but-set-variable -Wno-unused-function -Isrc -Iout -o out/code_gen.exe app/code_gen.c
.\out\code_gen.exe > .\out\generated.h
clang -march=native -O0 -g -Wall -Werror -Wno-unused-variable -Wno-unused-but-set-variable -Wno-unused-function -Isrc -Iout -o out/hello.exe app/hello.c
clang -march=native -O0 -g -Wall -Werror -Wno-unused-variable -Wno-unused-but-set-variable -Wno-unused-function -Isrc -Iout -o out/time_cmd.exe app/time_cmd.c
clang -march=native -O0 -g -Wall -Werror -Wno-unused-variable -Wno-unused-but-set-variable -Wno-unused-function -Isrc -Iout -o out/quest_for_nothing.exe app/quest_for_nothing.c
clang -march=native -O0 -g -Wall -Werror -Wno-unused-variable -Wno-unused-but-set-variable -Wno-unused-function -Isrc -Iout -o out/hot.exe app/hot.c
