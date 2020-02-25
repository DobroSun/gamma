#include "gamma/manager.hpp"
#include "gamma/action_state.hpp"

#include <SDL2/SDL.h>
#include <cassert>

MainManager::MainManager(): 
    state(new DefaultState) {
}

MainManager::~MainManager() {
  assert(state && "Cannot delete non existing state!");
  delete state;
}


void MainManager::set_state(SDL_Event event) {
  if(event.type == SDL_QUIT) {
    delete state;
    state = new ExitState();
  } else {
  }
}

bool MainManager::handle_action() {
  return state->handle_action();
}
