#include "gamma/pch.h"
#include "gamma/init.h"
#include "gamma/globals.h"

static SDL_Window *win = nullptr;

int Init_SDL() {
  if(SDL_Init(SDL_INIT_EVENTS | SDL_INIT_TIMER)) {
    fprintf(stderr, "%s: %s", "Error Initializing SDL.\n", SDL_GetError());
    return 1;
  }
  if(TTF_Init()) {
    fprintf(stderr, "%s: %s", "Error Initializing TTF.\n", TTF_GetError());
    return 1;
  }

  win = SDL_CreateWindow("Gamma",
                         SDL_WINDOWPOS_CENTERED,
                         SDL_WINDOWPOS_CENTERED,
                         Width, Height,
                         SDL_WINDOW_RESIZABLE);
  if(!win) {
    fprintf(stderr, "%s: %s", "Error Creating SDL_Window.\n", SDL_GetError());
    return 1;
  }
  return 0;
}

SDL_Window *get_win() {
  return win;
}
