#include "inc.h"
#include "str.h"

#include <stdio.h>


static void test(char *s) {
  printf("%s = %d\n", s, str_len(s));
}

int main(void) {
  test("Hello World");
  test("123456");
}
