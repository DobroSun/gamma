#include "gamma/pch.h"
#include "gamma/timer.h"
#include "gamma/globals.h"
#include "gamma/cursor.h"

static int tw = 0, th = 0;
static inline Uint32 f(Uint32 interval) {
  return interval;
}


void update_cursor(SDL_Renderer *renderer, SDL_Texture *t, int i_pixels, int j_pixels) {
  // Copy&Paste.
  SDL_QueryTexture(t, nullptr, nullptr, &tw, &th);
  SDL_Rect dst {TextLeftBound + j_pixels, TextUpperBound + i_pixels, tw, th};
  SDL_RenderCopy(renderer, t, nullptr, &dst);
}


SDL_TimerID StartTimer(Uint32 delay) {
  char junk = 0;
  SDL_TimerID cursortimer = SDL_AddTimer(delay, (callback)f, &junk);
  return cursortimer;
}
