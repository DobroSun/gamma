#ifndef GAMMA_INIT_H
#define GAMMA_INIT_H

int Init_SDL() {
  if(SDL_Init(SDL_INIT_EVENTS | SDL_INIT_TIMER)) {
    fprintf(stderr, "%s: %s", "Error Initializing SDL", SDL_GetError());
    return 1;
  }
  if(TTF_Init()) {
    fprintf(stderr, "%s: %s", "Error Initializing TTF", TTF_GetError());
    return 1;
  }
  return 0;
}


#endif
