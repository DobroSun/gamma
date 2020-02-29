#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "exit_mock_sdl_impl.hpp"
#include "exit_mock_manager.hpp"
#include "tests/mock_fact.hpp"
#include "gamma/window.hpp"

using ExitMockFactory = MockFactory<ExitMockManager, ExitMockSdlImpl>;

TEST(ExitWindowCase, test_return_value) {
  ExitMockManager *manager; ExitMockSdlImpl *sdl_impl;
  ExitMockFactory factory;


  std::unique_ptr<Window> win(new ExitWindow(&factory));

	manager = dynamic_cast<ExitMockManager*>(win->get_manager());
	sdl_impl = dynamic_cast<ExitMockSdlImpl*>(win->get_sdl_impl());
  

  ON_CALL(*sdl_impl, poll_event).
    WillByDefault(Return(true));
  ON_CALL(*sdl_impl, is_current_win).
    WillByDefault(Return(true));

  manager->RealSetState();
  manager->RealHandleAction();

  bool result = win->run();
  ASSERT_EQ(!result, 1);
}
