#ifndef GAMMA_SCROLL_BAR_H
#define GAMMA_SCROLL_BAR_H
#include "gamma/globals.h"
#include "gamma/fwd_decl.h"

struct ScrollBar {
  int h = Height - TextUpperBound - TextBottomBound;
  int w = 25;
  int startx = Width - w;
  int starty = TextUpperBound;

  int start = 0;
  int small_h = h/20;
  char clicky = 0;
};

bool clicked_bar(const ScrollBar &bar, double x, double y);
bool clicked_small(const ScrollBar &bar, double x, double y);

void reinit_bar(ScrollBar &bar);
void draw_bar(const ScrollBar &bar, SDL_Renderer *renderer);
void got_clicked(ScrollBar &bar, double x, double y);
void update_bar(const buffer_view &buffer, ScrollBar &bar, Uint32 start);
void update_start(const buffer_view &buffer, ScrollBar*& bar, double y, Uint32 &);

#endif
