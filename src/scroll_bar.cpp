#include "gamma/pch.h"
#include "gamma/scroll_bar.h"
#include "gamma/utility.h"
#include "gamma/view.h"


// Handles resize action.
void reinit_bar(ScrollBar &bar) {
  auto tmp = bar.start / static_cast<double>(bar.h);

  const ScrollBar bb;
  bar = bb;
  bar.start = tmp * bb.h;
  // bar.start == start * bar.h / buffer.size();
  // See update_bar method.
}

static void draw_smallbar(const ScrollBar &bar, SDL_Renderer *renderer) {
  SDL_SetRenderDrawColor(renderer, 200, 100, 40, 255); 
  SDL_Rect rr {bar.startx, bar.start+bar.starty, bar.w, bar.small_h};
  SDL_RenderFillRect(renderer, &rr);
}

void draw_bar(const ScrollBar &bar, SDL_Renderer *renderer) {
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); 
  SDL_Rect rr {bar.startx, bar.starty, bar.w, bar.h};
  SDL_RenderFillRect(renderer, &rr);
  draw_smallbar(bar, renderer);
}

bool clicked_bar(const ScrollBar &bar, double x, double y) {
  auto &starty = bar.starty;
  return y >= starty && x >= bar.startx && y < starty+bar.h;
}

bool clicked_small(const ScrollBar &bar, double x, double y) {
  auto starty = bar.start+TextUpperBound;
  return y >= starty && x >= bar.startx && y < starty+bar.small_h;
}

void got_clicked(ScrollBar &bar, double x, double y) {
  bar.clicky = y-TextUpperBound-bar.start;
}

void update_bar(const buffer_view &buffer, ScrollBar &bar, Uint32 start) {
  bar.start = (start * bar.h / buffer.size());
}
void update_start(const buffer_view &buffer, ScrollBar*& bar, double y, Uint32 &s) {
  auto &clicked = bar->clicky;
  auto tmp = (y < clicked)? clicked: y;

  Uint32 r = tmp * buffer.size() / bar->h;
  auto maxr = buffer.size()-numrows();

  if(r > maxr) {
    s = maxr;
  } else {
    bar->start = tmp-bar->clicky;
    s = r;
  }
}
