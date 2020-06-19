#ifndef GAMMA_CURSOR_H
#define GAMMA_CURSOR_H
#include "gamma/fwd_decl.h"
struct Cursor {
  int i, j;
};

bool operator==(const Cursor &c1, const Cursor &c2);
bool operator!=(const Cursor &c1, const Cursor &c2);


void fix_cursor(const buffer_view &, Cursor &);
void move_cursor(buffer_view &, int &, int &, int, int);
#endif
