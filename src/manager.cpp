#include "gamma/gamma_factory.hpp"
#include "gamma/action_state.hpp"
#include "gamma/manager.hpp"

#include <SDL2/SDL.h>
#include <iostream>
#include <cassert>
#include <memory>

MainManager::MainManager(): 
    state(new DefaultState) {
}

MainManager::~MainManager() {
  assert(state && "Cannot delete non existing state!");
  delete state;
}


void MainManager::set_state(SdlImpl &sdl_impl, SDL_Event &event) {
  assert(state && "Cannot delete not existing state!");
  if(event.type == SDL_QUIT) {

    delete state;
    state = new ExitState(new ExitFactory);
  } else {
  }
}

bool MainManager::handle_action() {
  return state->handle_action();
}

ExitManager::ExitManager():
    state(new DefaultState) {
}

ExitManager::~ExitManager() {
  assert(state && "Cannot delete non exitsting state!");
  delete state;
}

void ExitManager::set_state(SdlImpl &sdl_impl, SDL_Event &event) {
  assert(state && "Cannot delete not existing state!");
  if(event.type == SDL_WINDOWEVENT &&
     event.window.event == SDL_WINDOWEVENT_CLOSE &&
     sdl_impl.is_current_win(event)) {
    
    delete state;
    state = new QuitState();
  } else {
  }
}

bool ExitManager::handle_action() {
  return state->handle_action();
}
