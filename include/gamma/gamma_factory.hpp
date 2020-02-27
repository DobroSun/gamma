#ifndef GAMMA_GAMMA_FACTORY_HPP
#define GAMMA_GAMMA_FACTORY_HPP
#include "gamma/sdl_impl.hpp"
#include "gamma/manager.hpp"
#include <memory>

class GammaFactory {
public:
  virtual ~GammaFactory() {}
  virtual std::unique_ptr<SdlImpl> create_sdl_impl() = 0;
  virtual std::unique_ptr<Manager> create_manager() = 0;
};


class MainFactory: public GammaFactory {
public:
  ~MainFactory() {}
  std::unique_ptr<SdlImpl> create_sdl_impl() {
    return std::unique_ptr<SdlImpl>(new MainSdlImpl);
  }
  std::unique_ptr<Manager> create_manager() {
    return std::unique_ptr<Manager>(new MainManager);
  }
};

class ExitFactory: public GammaFactory {
public:
  ~ExitFactory() {}
  std::unique_ptr<SdlImpl> create_sdl_impl() {
    return std::unique_ptr<SdlImpl>(new ExitSdlImpl);
  }
  std::unique_ptr<Manager> create_manager() {
    return std::unique_ptr<Manager>(new ExitManager);
  }
};
#endif
