#ifndef GAMMA_TIMER_H
#define GAMMA_TIMER_H
using Uint32 = unsigned int;
typedef Uint32 (*callback)(Uint32, void*);
struct SDL_Texture;
struct Cursor;


namespace timer {
  inline void (*update_cursor)(SDL_Texture *, const Cursor &, int width);
  void update_fake(SDL_Texture *, const Cursor &, int width);
  void update_real(SDL_Texture *t, const Cursor &c, int width);

  Uint32 change_cursor_state(Uint32 interval);
};


#define StartTimer() \
  Uint32 delay_show = 500; \
  timer::update_cursor = timer::update_real; \ 
  SDL_TimerID cursortimer = SDL_AddTimer(delay_show, (callback)timer::change_cursor_state, &start); \ 
  (void)cursortimer;

#endif
