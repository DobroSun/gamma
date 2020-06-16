#include "gamma/pch.h"
#include "gamma/cursor.h"
#include "gamma/globals.h"
#include "gamma/utility.h"
#include "gamma/view.h"


bool operator==(const Cursor &c1, const Cursor &c2) {
  return c1.i == c2.i && c1.j == c2.j;
}
bool operator!=(const Cursor &c1, const Cursor &c2) {
  return c1.i != c2.i || c1.j != c2.j;
}

void get_pos(double x, double y, int fw, Cursor &c) {
  auto xx = x - TextLeftBound;
  auto yy = y - TextUpperBound;
  c.i = (yy + TextBottomBound - fsize/2) / fsize;
  c.j = xx / fw;
}

void fix_cursor(const buffer_view &buffer, Cursor &c) {
  auto &i = c.i; auto &j = c.j;
  auto actual_size = buffer.at_or(i, gap_buffer<char>{}).size();

  j = (j > actual_size)? actual_size: j;
}
