#ifndef GAMMA_SCROLL_BAR_H
#define GAMMA_SCROLL_BAR_H

struct ScrollBar {
  int h = Height - TextUpperBound - TextBottomBound;
  int w = 25;
  int startx = Width - w;
  int starty = TextUpperBound;

  int start = 0;
  int small_h = 40;
};

void draw_smallbar(const ScrollBar &bar, SDL_Renderer *renderer) {
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

void update_bar(const String &buffer, ScrollBar &bar, Uint32 start) {
  bar.start = start * bar.h / buffer.size();
}
Uint32 update_start(const String &buffer, ScrollBar *bar, double y) {
  auto tmp = (y < 0)? 0: y;

  Uint32 r = tmp * buffer.size() / bar->h;
  auto maxr = buffer.size()-numrows();

  if(r > maxr) {
    return maxr;
  } else {
    bar->start = tmp;
    return r;
  }
}

#endif
