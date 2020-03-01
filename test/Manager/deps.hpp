#ifndef GAMMA_TESTS_MANAGER_DEPS_HPP
#define GAMMA_TESTS_MANAGER_DEPS_HPP
#include "tests/mock_manager.hpp"
#include "tests/mock_sdl.hpp"

#include "gamma/manager.hpp"
#include "gmock/gmock.h"
#include <SDL2/SDL.h>
#include <iostream>
using namespace ::testing;

class SdlImpl;
class ExitMockManager: public MockManager {
  ExitManager real;
public:
  void RealSetQuitState() {
    ON_CALL(*this, set_state).
      WillByDefault([this](SdlImpl &sdl_impl, SDL_Event) {
        SDL_Event sdl_event = {};
        sdl_event.type = SDL_WINDOWEVENT;
        sdl_event.window.event = SDL_WINDOWEVENT_CLOSE;

        real.set_state(sdl_impl, sdl_event);
      });
  }
  void RealSetDefaultState() {
    ON_CALL(*this, set_state).
      WillByDefault([this](SdlImpl &sdl_impl, SDL_Event) {
        SDL_Event sdl_event = {};
        sdl_event.type = 0;

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


class MainMockManager: public MockManager {
  MainManager real;
public:
  void RealSetQuitState() {
    ON_CALL(*this, set_state).
      WillByDefault([this](SdlImpl &sdl_impl, SDL_Event) {
        SDL_Event sdl_event = {};
        sdl_event.type = SDL_QUIT;

        real.set_state(sdl_impl, sdl_event);
      });
  }
  void RealSetDefaultState() {
    ON_CALL(*this, set_state).
      WillByDefault([this](SdlImpl &sdl_impl, SDL_Event) {
        SDL_Event sdl_event = {};
        sdl_event.type = 0;

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

class MMockSdlImpl: public MockSdlImpl {
};
class MMockCanvas: public MockCanvas {
};
#endif
