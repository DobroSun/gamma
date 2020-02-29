#include "gamma/window.hpp"
#include "gamma/gamma_factory.hpp"


#include <SDL2/SDL.h>
#include <iostream>
#include <cassert>
#include <memory>


ExitWindow::ExitWindow(GammaFactory *fact):
    sdl_impl(fact->create_sdl_impl()),
    manager(fact->create_manager()) {
  assert(sdl_impl && "SdlImpl hasn't been initialized!");
  assert(manager && "Manager hasn't been initialized!");
}

ExitWindow::~ExitWindow() {}

Manager *ExitWindow::get_manager() {
  return manager.get();
};
SdlImpl *ExitWindow::get_sdl_impl() {
  return sdl_impl.get();
};

bool ExitWindow::run() {
  bool is_running = true;
  SDL_Event event;
  while(is_running) {
    while(sdl_impl->poll_event(event)) {
      manager->set_state(*sdl_impl, event);

      is_running = manager->handle_action();
      if(!is_running) break;
    }
  }
  return is_running;
}


MainWindow::MainWindow(GammaFactory *fact):
    gamma_factory(fact),
    sdl_impl(fact->create_sdl_impl()),
    manager(fact->create_manager()) {
  assert(sdl_impl && "SdlImpl hasn't been initialized!");
  assert(manager && "Manager hasn't been initialized!");
}

MainWindow::~MainWindow() {}

Manager *MainWindow::get_manager() {
  return manager.get();
};
SdlImpl *MainWindow::get_sdl_impl() {
  return sdl_impl.get();
};

bool MainWindow::run() {
  bool is_running = true;
  sdl_impl->set_window_resizable(SDL_TRUE);

  SDL_Event event;
  while(is_running) {
    while(sdl_impl->poll_event(event)) {
      manager->set_state(*sdl_impl, event);

      is_running = manager->handle_action();
      if(!is_running) break;
    }
  }
  return true;
}
