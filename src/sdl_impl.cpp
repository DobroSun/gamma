#include "gamma/sdl_impl.hpp"
#include "gamma/globals.hpp"

#include <cassert>


// FIXME:
// Somehow refactor copying methods.
MainSdlImpl::MainSdlImpl() {
  win = SDL_CreateWindow("",
              SDL_WINDOWPOS_CENTERED,
              SDL_WINDOWPOS_CENTERED,
              WIDTH, HEIGHT, 0);
  renderer = SDL_CreateRenderer(win, -1, 0);
  
  assert(win && "Couldn't create window");
  assert(renderer && "Couldn't create renderer");
}

MainSdlImpl::~MainSdlImpl() {
  assert(win && "Window already doesnt' exists, cannot delete");
  assert(renderer && "Renderer already doesnt' exists, cannot delete");
  SDL_DestroyWindow(win);
  SDL_DestroyRenderer(renderer);
}

bool MainSdlImpl::poll_event(SDL_Event &event) {
  return SDL_PollEvent(&event);
}

void MainSdlImpl::set_window_resizable(SDL_bool resizable) {
  SDL_SetWindowResizable(win, resizable);
}

bool MainSdlImpl::is_current_win(SDL_Event &event) {
  return (SDL_GetWindowID(win) == event.window.windowID);
};

ExitSdlImpl::ExitSdlImpl() {
  win = SDL_CreateWindow("Are you sure want to exit?",
                  SDL_WINDOWPOS_CENTERED,
                  SDL_WINDOWPOS_CENTERED,
                  400, 200, 0);
  renderer = SDL_CreateRenderer(win, -1, 0);
  
  assert(win && "Couldn't create window");
  assert(renderer && "Couldn't create renderer");
};

ExitSdlImpl::~ExitSdlImpl() {
  assert(win && "Window already doesnt' exists, cannot delete");
  assert(renderer && "Renderer already doesnt' exists, cannot delete");
  SDL_DestroyWindow(win);
  SDL_DestroyRenderer(renderer);
}

bool ExitSdlImpl::poll_event(SDL_Event &event) {
  return SDL_PollEvent(&event);
}

void ExitSdlImpl::set_window_resizable(SDL_bool resizable) {
  SDL_SetWindowResizable(win, resizable);
}

bool ExitSdlImpl::is_current_win(SDL_Event &event) {
  return (SDL_GetWindowID(win) == event.window.windowID);
};
