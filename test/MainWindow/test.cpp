#include "tests/test_pch.hpp"

#include "tests/mock_fact.hpp"
#include "gamma/window.hpp"
#include "deps.hpp"

using namespace ::testing;
using MainMockFactory = MockFactory<MainMockManager, MainMockSdlImpl, MainMockCanvas>;

TEST(MainWindowCase, test_return_value1) {
  MainMockManager *manager; MainMockSdlImpl *sdl_impl;
  MainMockFactory factory;


  std::unique_ptr<Window> win(new MainWindow(&factory));

	manager = dynamic_cast<MainMockManager*>(win->get_manager());
	sdl_impl = dynamic_cast<MainMockSdlImpl*>(win->get_sdl_impl());
  
  ON_CALL(*sdl_impl, poll_event)
    .WillByDefault(Return(true));
  ON_CALL(*manager, handle_action)
    .WillByDefault(Return(false));
  
  manager->RealSetState();

  bool result = win->run();
  ASSERT_EQ(!result, 0);
}
/*
TEST(MainWindowCase, test_return_value2) {
  MainMockManager *manager; MainMockSdlImpl *sdl_impl;
  MainMockFactory factory;


  std::unique_ptr<Window> win(new MainWindow(&factory));

	manager = dynamic_cast<MainMockManager*>(win->get_manager());
	sdl_impl = dynamic_cast<MainMockSdlImpl*>(win->get_sdl_impl());
  

  bool is_called = false; 
  ON_CALL(*sdl_impl, poll_event)
    .WillByDefault(Invoke(
      [&is_called](SDL_Event&) -> bool {
        if(!is_called) {
          is_called = true;
          return true;
        }
        return false;
      }
    ));
  ON_CALL(*sdl_impl, set_window_resizable)
    .WillByDefault(Return());
  ON_CALL(*manager, set_state)
    .WillByDefault(Return());
  
  EXPECT_CALL(*sdl_impl, poll_event)
    .WillOnce(Return(true))
    .WillRepeatedly(Return(false));

  manager->RealHandleAction();

  bool result = win->run();
  ASSERT_EQ(!result, 0);
}
*/
