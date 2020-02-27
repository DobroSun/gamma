#ifndef GAMMA_TESTS_MOCK_FACTORY_HPP
#define GAMMA_TESTS_MOCK_FACTORY_HPP
#include "gamma/gamma_factory.hpp"
#include "tests/mock_manager.hpp"
#include "tests/mock_sdl.hpp"

#include "gmock/gmock.h"
#include <memory>

template<class MockManagerRealization, class MockSdlImplRealization>
class MockFactory: public GammaFactory {
public:
  std::unique_ptr<SdlImpl> create_sdl_impl() {
    return std::make_unique<MockSdlImplRealization>();
  }
  std::unique_ptr<Manager> create_manager() {
    return std::make_unique<MockManagerRealization>();
  }
};
#endif
