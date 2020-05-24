#include "gamma/pch.h"
#include "gamma/timer.h"
#include "gamma/globals.h"
extern SDL_Renderer *renderer;
struct Cursor {
  int i, j;
};


namespace timer {
  bool is_shown = true;
  bool paused = true;
  void update_fake(SDL_Texture *, const Cursor &, int) {
  }

  void update_real(SDL_Texture *t, const Cursor &c, int width) {
    int tw = 0, th = 0;
    SDL_QueryTexture(t, nullptr, nullptr, &tw, &th);
    SDL_Rect dst {TextLeftBound+width*c.j, TextUpperBound+fsize*c.i, tw, th};
    SDL_RenderCopy(renderer, t, nullptr, &dst);
  }


  Uint32 change_cursor_state(Uint32 interval) {
    if(paused) return interval;

    if(is_shown) {
      update_cursor = update_real;
    } else {
      update_cursor = update_fake;
    }
    is_shown = !is_shown;
    return interval;
  }
}; // namespace

SDL_TimerID StartTimer(Uint32 delay) {
  Uint32 junk = 0;
  timer::update_cursor = timer::update_real;
  SDL_TimerID cursortimer = SDL_AddTimer(delay, (callback)timer::change_cursor_state, &junk);
  return cursortimer;
}

void PauseTimer() {
  timer::paused = true;
}

void ResumeTimer() {
  timer::paused = false;
}
