#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <iostream>
#include <type_traits>

#include "gamma/window.hpp"
#include "tests/mock_fact.hpp"
#include "tests/utils.hpp"
#include <SDL2/SDL.h>
using namespace ::testing;


TEST(MyCase, test_closing_window) {
	MockManager *manager; MockSdlImpl *sdl_impl;

  std::unique_ptr<GammaFactory> factory(new MockFactory());
	std::unique_ptr<Window> win(new MainWindow(factory.get()));
/*
	manager = win->get_manager();
	sdl_impl = win->get_sdl_impl();
*/
  win->run();
  //.WillByDefault(Invoke(&Add, &MockAdd::add_real));
}


int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
