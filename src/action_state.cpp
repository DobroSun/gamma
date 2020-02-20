#include "gamma/action_state.hpp"
#include "gamma/window.hpp"

bool ExitState::handle_action() {
  std::unique_ptr<Window> win(new ExitWindow());
  return win->run();
}
