#ifndef GAMMA_TESTS_MOCK_SDLIMPL_HPP
#define GAMMA_TESTS_MOCK_SDLIMPL_HPP
#include "gamma/sdl_impl.hpp"
#include "gmock/gmock.h"

class MockSdlImpl: public SdlImpl {
public:
  MOCK_METHOD(bool, poll_event, (SDL_Event* event));
  MOCK_METHOD(void, set_window_resizable, (SDL_bool resizable));
};
#endif
