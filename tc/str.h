#pragma once

// Simplicity
// - no 'const' chars, we don't use 'const'
#include "inc.h"

// Number of chars in a zero terminated string
static u32 str_len(char *s) {
  u32 n = 0;
  while(*s++) n++;
  return n;
}
