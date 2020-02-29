#ifndef GAMMA_SDL_IMPLEMENTATION
#define GAMMA_SDL_IMPLEMENTATION
#include <SDL2/SDL.h>


class SdlImpl {
public:
  virtual ~SdlImpl() {}
  virtual bool poll_event(SDL_Event &event) = 0;
  virtual bool is_current_win(SDL_Event &event) = 0;
};

struct SDL_Window;
struct SDL_Renderer;
class MainSdlImpl: public SdlImpl {
  SDL_Window *win;
  SDL_Renderer *renderer;
public:
  ~MainSdlImpl();
  MainSdlImpl();
  bool poll_event(SDL_Event &event);
  bool is_current_win(SDL_Event &event);
};

class ExitSdlImpl: public SdlImpl {
  SDL_Window *win;
  SDL_Renderer *renderer;
public:
  ~ExitSdlImpl();
  ExitSdlImpl();
  bool poll_event(SDL_Event &event);
  bool is_current_win(SDL_Event &event);
};
#endif
