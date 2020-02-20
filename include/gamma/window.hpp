#ifndef GAMMA_WINDOW_HPP
#define GAMMA_WINDOW_HPP
#include <memory>

class Window {
public:
  virtual ~Window() {}
  virtual bool run() = 0;
};


class SdlImpl;
class Manager;
class GammaFactory;
class MainWindow: public Window {
  GammaFactory &gamma_factory;
  std::unique_ptr<SdlImpl> sdl_impl;
  std::unique_ptr<Manager> manager;
  bool is_running;
public:
  MainWindow(GammaFactory &fact);
  ~MainWindow();
  bool run();
};

class IRendererImpl;
struct SDL_Window;
class ExitWindow: public Window {
  IRendererImpl *sdl_renderer;
  SDL_Window *sdl_win;
public:
  ExitWindow();
  ~ExitWindow();
  bool run();
};
#endif
