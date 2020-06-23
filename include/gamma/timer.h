#ifndef GAMMA_TIMER_H
#define GAMMA_TIMER_H
#include "gamma/fwd_decl.h"

inline void (*update_cursor)(SDL_Renderer *, SDL_Texture *, int, int);
void update_fake(SDL_Renderer *, SDL_Texture *, int, int);
void update_real(SDL_Renderer *, SDL_Texture *t, int, int);

SDL_TimerID StartTimer(Uint32);
#endif
