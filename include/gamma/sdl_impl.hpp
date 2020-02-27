#ifndef GAMMA_SDL_IMPLEMENTATION
#define GAMMA_SDL_IMPLEMENTATION
#include <SDL2/SDL.h>


class SdlImpl {
  SDL_Window *win;
  SDL_Renderer *renderer;
public:
  virtual ~SdlImpl() {}
  virtual bool poll_event(SDL_Event *event) = 0;
  virtual void set_window_resizable(SDL_bool resizable) = 0;
};

struct SDL_Window;
struct SDL_Renderer;
class MainSdlImpl: public SdlImpl {
  SDL_Window *win;
  SDL_Renderer *renderer;
public:
  ~MainSdlImpl();
  MainSdlImpl();
  bool poll_event(SDL_Event *event);
  void set_window_resizable(SDL_bool resizable);
};

class ExitSdlImpl: public SdlImpl {
  SDL_Window *win;
  SDL_Renderer *renderer;
public:
  ~ExitSdlImpl();
  ExitSdlImpl();
  bool poll_event(SDL_Event *event);
  void set_window_resizable(SDL_bool resizable);
};
#endif
