//
#include "gamma/renderer.hpp"
#include "gamma/globals.hpp"
//
#include "gamma/window.hpp"
#include "gamma/gamma_factory.hpp"


#include <SDL2/SDL.h>
#include <iostream>
#include <cassert>
#include <memory>


ExitWindow::ExitWindow() {
  sdl_win = SDL_CreateWindow("Are you sure want to exit?",
                  SDL_WINDOWPOS_CENTERED,
                  SDL_WINDOWPOS_CENTERED,
                  400, 200, 0);
  assert(sdl_win && "Couldn't create window");

  // Abstract factory?
  // Just Why?
  sdl_renderer = new RendererImpl(sdl_win);
}

ExitWindow::~ExitWindow() {
  SDL_DestroyWindow(sdl_win);
  delete sdl_renderer;
}


bool ExitWindow::run() {
  SDL_Event event;
  while(1) {
    while(SDL_PollEvent(&event)) {
      if(event.type == SDL_WINDOWEVENT &&
         event.window.event == SDL_WINDOWEVENT_CLOSE) {
          return false;
      }
    }
  } 
}


MainWindow::MainWindow(GammaFactory &fact): 
    gamma_factory(fact),
    sdl_impl(fact.create_sdl_impl()),
    manager(fact.create_manager()),
    is_running(true) {}

MainWindow::~MainWindow() {
}

bool MainWindow::run() {
  sdl_impl->set_window_resizable(SDL_TRUE);

  SDL_Event event;
  while(is_running) {
    while(sdl_impl->poll_event(&event)) {
      manager->set_state(event.type);

      is_running = manager->handle_action();
      if(!is_running) break;
    }
  }
  return true;
}
