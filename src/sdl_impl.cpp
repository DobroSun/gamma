#include "gamma/sdl_impl.hpp"
#include "gamma/globals.hpp"

#include <cassert>
#include <stdint.h>


// FIXME:
// Somehow refactor copying methods.
MainSdlImpl::MainSdlImpl() {
  uint32_t flags = SDL_WINDOW_RESIZABLE;
  win = SDL_CreateWindow("",
              SDL_WINDOWPOS_CENTERED,
              SDL_WINDOWPOS_CENTERED,
              WIDTH, HEIGHT, flags);
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

bool MainSdlImpl::is_current_win(SDL_Event &event) {
  return (SDL_GetWindowID(win) == event.window.windowID);
};

ExitSdlImpl::ExitSdlImpl() {
  uint32_t flags = 0;
  win = SDL_CreateWindow("Are you sure want to exit?",
                  SDL_WINDOWPOS_CENTERED,
                  SDL_WINDOWPOS_CENTERED,
                  400, 200, flags);
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

bool ExitSdlImpl::is_current_win(SDL_Event &event) {
  return (SDL_GetWindowID(win) == event.window.windowID);
};
