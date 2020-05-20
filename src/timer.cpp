#include "gamma/pch.h"
#include "gamma/timer.h"
#include "gamma/globals.h"
extern SDL_Renderer *renderer;
struct Cursor {
  int i, j;
};


namespace timer {
  bool is_shown = true;
  void update_fake(SDL_Texture *, const Cursor &, int width) {
  }

  void update_real(SDL_Texture *t, const Cursor &c, int width) {
    int tw = 0, th = 0;
    SDL_QueryTexture(t, nullptr, nullptr, &tw, &th);
    SDL_Rect dst {TextLeftBound+width*c.j, TextUpperBound+fsize*c.i, tw, th};
    SDL_RenderCopy(renderer, t, nullptr, &dst);
  }


  Uint32 change_cursor_state(Uint32 interval) {
    if(is_shown) {
      update_cursor = update_real;
    } else {
      update_cursor = update_fake;
    }
    is_shown = !is_shown;
    return interval;
  }

}; // namespace
