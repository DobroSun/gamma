#ifndef GAMMA_TIMER_H
#define GAMMA_TIMER_H
#include "gamma/fwd_decl.h"


namespace timer {
  inline void (*update_cursor)(SDL_Renderer *, SDL_Texture *, const Cursor &, int width);
  void update_fake(SDL_Renderer *, SDL_Texture *, const Cursor &, int width);
  void update_real(SDL_Renderer *, SDL_Texture *t, const Cursor &c, int width);

  Uint32 change_cursor_state(Uint32 interval);
};

SDL_TimerID StartTimer(Uint32);
void PauseTimer();
void ResumeTimer();
#endif
