if not exist out mkdir out
clang -o out\build src\hot.c && out\build %*
