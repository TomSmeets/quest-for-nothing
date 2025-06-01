if not exist out mkdir out
clang -O0 -g -I src -o out\build-pre.exe src\build\build.c && out\build-pre.exe %*
