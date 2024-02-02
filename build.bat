if not exist out mkdir out
set args=-march=native -O0 -g -Wall -Werror -Wno-unused-variable -Wno-unused-but-set-variable -Wno-unused-function -Wno-format -Isrc -Iout -Wl,/INCREMENTAL:NO

clang %args%         -o out/code_gen.exe          app/code_gen.c
.\out\code_gen.exe > .\out\generated.h
clang %args%         -o out/hello.exe             app/hello.c
clang %args%         -o out/time_cmd.exe          app/time_cmd.c
clang %args%         -o out/quest_for_nothing.exe app/quest_for_nothing.c
clang %args%         -o out/hot.exe               app/hot.c
clang %args% -Wl,-export:main_init,-export:main_update -shared -o out/quest_for_nothing.dll app/quest_for_nothing.c
touch out/trigger
