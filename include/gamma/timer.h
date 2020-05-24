#ifndef GAMMA_TIMER_H
#define GAMMA_TIMER_H
using Uint32 = unsigned int;
extern SDL_Renderer *renderer;
typedef Uint32 (*callback)(Uint32, void*);
struct SDL_Texture;
struct Cursor;


namespace timer {
  inline void (*update_cursor)(SDL_Texture *, const Cursor &, int width);
  void update_fake(SDL_Texture *, const Cursor &, int width);
  void update_real(SDL_Texture *t, const Cursor &c, int width);

  Uint32 change_cursor_state(Uint32 interval);
};

SDL_TimerID StartTimer(Uint32);
void PauseTimer();
void ResumeTimer();
#endif
