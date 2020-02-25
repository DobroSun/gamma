#ifndef GAMMA_WINDOW_IMPL_HPP
#define GAMMA_WINDOW_IMPL_HPP
#include "gamma/sdl_impl.hpp"
#include "gamma/manager.hpp"

class WindowImpl: public SdlImpl, public Manager {
public:
  virtual WindowImpl() {}
};
#endif
