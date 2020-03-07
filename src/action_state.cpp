#include "gamma/pch.hpp"

#include "gamma/action_state.hpp"
#include "gamma/gamma_factory.hpp"
#include "gamma/window.hpp"

ExitState::ExitState(GammaFactory *fact_): fact(fact_) {}
ExitState::~ExitState() {
  assert(fact && "Cannot delete not existing factory");
  delete fact;
}

bool ExitState::handle_action() {
  std::unique_ptr<Window> win(new ExitWindow(fact));
  return win->run();
}
