#ifndef GAMMA_TESTS_MOCK_MANAGER_HPP
#define GAMMA_TESTS_MOCK_MANAGER_HPP
#include "gamma/manager.hpp"
#include "gmock/gmock.h"

union SDL_Event;
class MockManager: public Manager {
public:
  MOCK_METHOD(void, set_state, (SDL_Event event));
  MOCK_METHOD(bool, handle_action, ());
};

#endif
