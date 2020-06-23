#include "gamma/pch.h"
#include "gamma/timer.h"
#include "gamma/globals.h"
#include "gamma/cursor.h"

static int tw = 0, th = 0;
static bool is_shown = true;

static Uint32 change_cursor_state(Uint32 interval) {
  if(is_shown) {
    update_cursor = update_real;
  } else {
    update_cursor = update_fake;
  }
  is_shown = !is_shown;
  return interval;
}


void update_fake(SDL_Renderer *renderer, SDL_Texture *, int, int) {
}

void update_real(SDL_Renderer *renderer, SDL_Texture *t, int i_pixels, int j_pixels) {
  SDL_QueryTexture(t, nullptr, nullptr, &tw, &th);
  SDL_Rect dst {TextLeftBound + j_pixels, TextUpperBound + i_pixels, tw, th};
  SDL_RenderCopy(renderer, t, nullptr, &dst);
}


SDL_TimerID StartTimer(Uint32 delay) {
  char junk = 0;
  update_cursor = update_real;
  SDL_TimerID cursortimer = SDL_AddTimer(delay, (callback)change_cursor_state, &junk);
  return cursortimer;
}
