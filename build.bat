if not exist out mkdir out
clang -o out\build src\build.c && out\build %*
