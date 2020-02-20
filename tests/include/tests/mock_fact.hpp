#ifndef GAMMA_TESTS_MOCK_FACTORY_HPP
#define GAMMA_TESTS_MOCK_FACTORY_HPP
#include "gamma/gamma_factory.hpp"
#include "tests/mock_sdl.hpp"
#include "gmock/gmock.h"

#include <memory>

class MockFactory: public GammaFactory {
public:
  //MOCK_METHOD(std::unique_ptr<SdlImpl>, create_sdl_impl, ());
  //std::unique_ptr<SdlImpl> create_sdl_impl() {
    //return std::make_unique<SdlImpl>();
  //}
};

#endif
