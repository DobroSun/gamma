#ifndef GAMMA_TESTS_MAINWINDOW_DEPS_HPP
#define GAMMA_TESTS_MAINWINDOW_DEPS_HPP
#include "tests/mock_manager.hpp"
#include "tests/mock_sdl.hpp"
#include "tests/mock_canvas.hpp"

#include "gamma/manager.hpp"
#include "gmock/gmock.h"
#include <SDL2/SDL.h>
using namespace ::testing;

class SdlImpl;
class MainMockManager: public MockManager {
  MainManager real;
public:
  void RealSetState() {
    ON_CALL(*this, set_state)
      .WillByDefault([this](SdlImpl &sdl_impl, SDL_Event) {
        SDL_Event sdl_event = {};
        sdl_event.type = SDL_QUIT;

        real.set_state(sdl_impl, sdl_event);
      });
  }
  void RealHandleAction() {
    ON_CALL(*this, handle_action)
      .WillByDefault([this]() {
        return real.handle_action();
      });
  }
};

class MainMockSdlImpl: public MockSdlImpl {
};
class MainMockCanvas: public MockCanvas {
};
#endif
