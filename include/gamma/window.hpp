#ifndef GAMMA_WINDOW_HPP
#define GAMMA_WINDOW_HPP
class Window {
public:
  virtual ~Window() {}
  virtual bool run() = 0;
};


struct SDL_Window;
class GameWindow: public Window {
  SDL_Window *sdl_win;
  bool is_running;
public:
  GameWindow();
  ~GameWindow();
  bool run();
};

class ExitWindow: public Window {
  SDL_Window *sdl_win;
public:
  ExitWindow();
  ~ExitWindow();
  bool run();
};
#endif
