if not exist out mkdir out
clang -O0 -g -o out\build-pre.exe src\build.c && out\build-pre.exe %*
