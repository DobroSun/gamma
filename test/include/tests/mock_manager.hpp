#ifndef GAMMA_TESTS_MOCK_MANAGER_HPP
#define GAMMA_TESTS_MOCK_MANAGER_HPP
#include "gamma/manager.hpp"
#include "gmock/gmock.h"

class SdlImpl;
union SDL_Event;
class MockManager: public Manager {
public:
  MOCK_METHOD(void, set_state, (SdlImpl &sdl_impl, SDL_Event &event), (override));
  MOCK_METHOD(bool, handle_action, (), (override));
};

#endif
