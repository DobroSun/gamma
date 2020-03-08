#include "tests/test_pch.hpp"

#include "tests/mock_fact.hpp"
#include "gamma/window.hpp"
#include "deps.hpp"


TEST(ManagerCase, test_handle_default_action) {
  NiceMock<MMockSdlImpl> sdl_impl; NiceMock<MMockCanvas> canvas;
  std::unique_ptr<Manager> manager(new MainManager);
  std::unique_ptr<Manager> ex_manager(new ExitManager);

  ON_CALL(sdl_impl, is_current_win)
    .WillByDefault(Return(false));


  SDL_Event sdl_event = {};
  sdl_event.type = 0;

  bool is_running;
  manager->set_state(sdl_impl, sdl_event);
  is_running = manager->handle_action();
  ASSERT_TRUE(is_running);

  ex_manager->set_state(sdl_impl, sdl_event);
  is_running = ex_manager->handle_action();
  ASSERT_TRUE(is_running);

}

TEST(ManagerCase, test_handle_default_exit_action) {
  NiceMock<MMockSdlImpl> sdl_impl; NiceMock<MMockCanvas> canvas;
  std::unique_ptr<Manager> ex_manager(new ExitManager);
  
  SDL_Event sdl_event = {};
  sdl_event.type = SDL_WINDOWEVENT;
  sdl_event.window.event = SDL_WINDOWEVENT_CLOSE;
  ON_CALL(sdl_impl, is_current_win)
    .WillByDefault(Return(true));

  bool is_running;
  ex_manager->set_state(sdl_impl, sdl_event);
  is_running = ex_manager->handle_action();
  ASSERT_FALSE(is_running);
}
