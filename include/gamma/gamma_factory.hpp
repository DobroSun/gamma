#ifndef GAMMA_GAMMA_FACTORY_HPP
#define GAMMA_GAMMA_FACTORY_HPP
#include "gamma/sdl_impl.hpp"
#include "gamma/manager.hpp"
#include "gamma/canvas.hpp"

class GammaFactory {
public:
  virtual ~GammaFactory() {}
  virtual SdlImpl *create_sdl_impl() = 0;
  virtual Manager *create_manager() = 0;
  virtual Canvas *create_canvas() = 0;
};


class MainFactory: public GammaFactory {
public:
  ~MainFactory() {}
  SdlImpl *create_sdl_impl() {
    return new MainSdlImpl;
  }
  Manager *create_manager() {
    return new MainManager;
  }
  Canvas *create_canvas() {
    return new MainCanvas;
  }
};

class ExitFactory: public GammaFactory {
public:
  ~ExitFactory() {}
  SdlImpl *create_sdl_impl() {
    return new ExitSdlImpl;
  }
  Manager *create_manager() {
    return new ExitManager;
  }
  Canvas *create_canvas() {
    return new ExitCanvas;
  }
};
#endif
