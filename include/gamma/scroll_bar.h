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
  int clicky = 0;
  int small_h;

  ScrollBar(long unsigned buffer_size);
};

bool clicked_bar(const ScrollBar &, double, double);
bool clicked_small(const ScrollBar &, double, double);

void reinit_bar(ScrollBar &, const buffer_view &);
void draw_bar(const ScrollBar &, SDL_Renderer *);
void got_clicked(ScrollBar &, double, double);
void start_to_bar(const buffer_view &, ScrollBar &);
void bar_to_start(buffer_view &, ScrollBar*&, double);

#endif
