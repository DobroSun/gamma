#ifndef GAMMA_WINDOW_HPP
#define GAMMA_WINDOW_HPP
#include <memory>

#include "gamma/manager.hpp"
#include "gamma/sdl_impl.hpp"

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
};


class SdlImpl;
class Manager;
class GammaFactory;
class MainWindow: public Window {
  GammaFactory *gamma_factory;
  std::unique_ptr<SdlImpl> sdl_impl;
  std::unique_ptr<Manager> manager;
public:
  MainWindow(GammaFactory *fact);
  ~MainWindow();
  bool run();

  Manager *get_manager();
  SdlImpl *get_sdl_impl();
};

class ExitWindow: public Window {
  std::unique_ptr<SdlImpl> sdl_impl;
  std::unique_ptr<Manager> manager;
public:
  ExitWindow(GammaFactory *fact);
  ~ExitWindow();
  bool run();

  Manager *get_manager();
  SdlImpl *get_sdl_impl();
};
#endif
