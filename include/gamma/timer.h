#ifndef GAMMA_TIMER_H
#define GAMMA_TIMER_H
#include "gamma/fwd_decl.h"

void update_cursor(SDL_Renderer *, SDL_Texture *, int, int);
SDL_TimerID StartTimer(Uint32);
#endif
