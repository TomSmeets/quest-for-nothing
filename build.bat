if not exist out mkdir out
clang -O0 -g -I lib -o out\build-pre.exe src\build.c && out\build-pre.exe %*
