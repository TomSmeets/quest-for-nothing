#pragma  once
#include "mat.h"
#include "input.h"
#include "image.h"

typedef struct Gfx Gfx;

static Gfx *gfx_init(char *title);
static void gfx_quad(Gfx *gfx, m4s *mtx, Image *img);

static Input *gfx_begin(Gfx *gfx);
static void gfx_end(Gfx *gfx);
