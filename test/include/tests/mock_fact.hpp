#ifndef GAMMA_TESTS_MOCK_FACTORY_HPP
#define GAMMA_TESTS_MOCK_FACTORY_HPP
#include "gamma/gamma_factory.hpp"
#include "tests/mock_manager.hpp"
#include "tests/mock_sdl.hpp"
#include "tests/mock_canvas.hpp"

#include "gmock/gmock.h"

template<class MockManagerRealization, class MockSdlImplRealization, class MockCanvasRealization>
class MockFactory: public GammaFactory {
public:
  SdlImpl *create_sdl_impl() {
    return new MockSdlImplRealization;
  }
  Manager *create_manager() {
    return new MockManagerRealization;
  }
  Canvas *create_canvas() {
    return new MockCanvasRealization;
  }
};
#endif
