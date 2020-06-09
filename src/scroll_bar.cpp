#include "gamma/pch.h"
#include "gamma/scroll_bar.h"
#include "gamma/utility.h"
#include "gamma/view.h"


// Handles resize action.
void reinit_bar(ScrollBar &bar, const buffer_view &buffer) {
  auto tmp = bar.start / static_cast<double>(bar.h);

  const ScrollBar bb{buffer.size()};
  bar = bb;
  bar.start = tmp * bb.h;
  // bar.start == start * bar.h / buffer.size();
  // See start_to_bar method.
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

void start_to_bar(const buffer_view &buffer, ScrollBar &bar, Uint32 start) {
  bar.start = start * bar.h /** bar.expand*/ / buffer.size();
}
void bar_to_start(const buffer_view &buffer, ScrollBar*& bar, double y, Uint32 &start) {
  auto clicked = bar->clicky;
  auto &bar_start = bar->start;
  auto bar_h = bar->h;
  auto pos = (y < clicked)? clicked: y; // if user clicks and moves it over the bar.
  auto buffer_size = buffer.size()-1;

  Uint32 r = pos * buffer_size / bar_h;
  if(r > buffer_size) {
    start = buffer_size;

  } else {
    auto small_top = pos-clicked;
    auto bound = bar_h-bar->small_h;
    if(small_top > bound) {
      bar_start = bound;
      start = buffer_size;
    } else {
      bar_start = small_top;
      start = r;
    }
  }
}
