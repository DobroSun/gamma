#ifndef GAMMA_CURSOR_H
#define GAMMA_CURSOR_H
#include "gamma/fwd_decl.h"

struct Cursor {
  int i, j;
};

void move_cursor(buffer_view &, int &, int &, int, int);
#endif
