#ifndef GAMMA_TESTS_MOCK_CANVAS_HPP
#define GAMMA_TESTS_MOCK_CANVAS_HPP
#include "gamma/canvas.hpp"
#include "gmock/gmock.h"

class SdlImpl;
class MockCanvas: public Canvas {
public:
  MOCK_METHOD(void, init, (SdlImpl &sdl_impl), (override));
  MOCK_METHOD(void, update, (SdlImpl &sdl_impl), (override));
};
#endif
