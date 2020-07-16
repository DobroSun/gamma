#ifndef GAMMA_CURSOR_H
#define GAMMA_CURSOR_H
#include "gamma/fwd_decl.h"

struct Cursor {
  int i, j;
};

void fix_gap(buffer_view &);
#endif
