#ifndef GAMMA_TESTS_EXIT_MOCK_MANAGER_HPP
#define GAMMA_TESTS_EXIT_MOCK_MANAGER_HPP
#include "tests/mock_manager.hpp"
#include "gamma/manager.hpp"
#include "gmock/gmock.h"
#include <SDL2/SDL.h>
#include <iostream>
using namespace ::testing;

class ExitMockManager: public MockManager {
  ExitManager real;
public:
  void RealSetState() {
    ON_CALL(*this, set_state).
      WillByDefault([this](SDL_Event) {
        SDL_Event sdl_event = {};
        sdl_event.type = SDL_WINDOWEVENT;
        sdl_event.window.event = SDL_WINDOWEVENT_CLOSE;

        real.set_state(sdl_event);
      });
  }
  void RealHandleAction() {
    ON_CALL(*this, handle_action).
      WillByDefault([this]() {
        return real.handle_action();
      });
  }
};
#endif
