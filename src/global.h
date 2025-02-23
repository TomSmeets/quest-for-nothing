#include "types.h"

typedef struct {
    bool reload;
    void *app;
    void *fmt;
    void *rand;
    void *os;
    void *mem;
} Global;

static Global G;
