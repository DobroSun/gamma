#ifndef GAMMA_TESTS_EXITWINDOW_DEPS_HPP
#define GAMMA_TESTS_EXITWINDOW_DEPS_HPP
#include "tests/mock_manager.hpp"
#include "tests/mock_sdl.hpp"
#include "tests/mock_canvas.hpp"

#include "gamma/manager.hpp"
#include "gmock/gmock.h"
#include <SDL2/SDL.h>
using namespace ::testing;

class SdlImpl;
class ExitMockManager: public MockManager {
  ExitManager real;
public:
  void RealSetState() {
    ON_CALL(*this, set_state).
      WillByDefault([this](SdlImpl &sdl_impl, SDL_Event) {
        SDL_Event sdl_event = {};
        sdl_event.type = SDL_WINDOWEVENT;
        sdl_event.window.event = SDL_WINDOWEVENT_CLOSE;

        real.set_state(sdl_impl, sdl_event);
      });
  }
  void RealHandleAction() {
    ON_CALL(*this, handle_action).
      WillByDefault([this]() {
        return real.handle_action();
      });
  }
};

class ExitMockSdlImpl: public MockSdlImpl {
};

class ExitMockCanvas: public MockCanvas {
};
#endif
