if not exist out mkdir out
clang -O0 -g -I src -o out\build.exe src\build\build.c && out\build.exe %*
