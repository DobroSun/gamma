#ifndef GAMMA_WINDOW_HPP
#define GAMMA_WINDOW_HPP
#include <memory>

class SdlImpl;
class Manager;
class Canvas;
class GammaFactory;
class Window {
public:
  virtual ~Window() {}
  virtual bool run() = 0;

  // FIXME:
  // Really have to do this for creating every mock test?
  // That means I need getters for every
  // Dependence in class?
  virtual Manager *get_manager() = 0;
  virtual SdlImpl *get_sdl_impl() = 0;
  virtual Canvas *get_canvas() = 0;
};


class MainWindow: public Window {
  GammaFactory *gamma_factory;
  std::unique_ptr<SdlImpl> sdl_impl;
  std::unique_ptr<Manager> manager;
  std::unique_ptr<Canvas> canvas;
public:
  MainWindow(GammaFactory *fact);
  ~MainWindow();
  bool run();

  Manager *get_manager();
  SdlImpl *get_sdl_impl();
  Canvas *get_canvas();
};

class ExitWindow: public Window {
  std::unique_ptr<SdlImpl> sdl_impl;
  std::unique_ptr<Manager> manager;
  std::unique_ptr<Canvas> canvas;
public:
  ExitWindow(GammaFactory *fact);
  ~ExitWindow();
  bool run();

  Manager *get_manager();
  SdlImpl *get_sdl_impl();
  Canvas *get_canvas();
};
#endif
