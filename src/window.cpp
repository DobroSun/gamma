#include "gamma/pch.hpp"

#include "gamma/window.hpp"
#include "gamma/gamma_factory.hpp"


ExitWindow::ExitWindow(GammaFactory *fact):
    sdl_impl(fact->create_sdl_impl()),
    manager(fact->create_manager()),
    canvas(fact->create_canvas()) {
  assert(sdl_impl && "SdlImpl hasn't been initialized!");
  assert(manager && "Manager hasn't been initialized!");
  assert(canvas && "Canvas hasn't been initialized!");
}

ExitWindow::~ExitWindow() {}

Manager *ExitWindow::get_manager() {
  return manager.get();
};
SdlImpl *ExitWindow::get_sdl_impl() {
  return sdl_impl.get();
};
Canvas *ExitWindow::get_canvas() {
  return canvas.get();
};

bool ExitWindow::run() {
  bool is_running = true;
  canvas->init(*sdl_impl);

  SDL_Event event;
  while(is_running) {
    canvas->update(*sdl_impl);
    sdl_impl->show();

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
    manager(fact->create_manager()),
    canvas(fact->create_canvas()) {
  assert(sdl_impl && "SdlImpl hasn't been initialized!");
  assert(manager && "Manager hasn't been initialized!");
  assert(canvas && "Canvas hasn't been initialized!");
}

MainWindow::~MainWindow() {}

Manager *MainWindow::get_manager() {
  return manager.get();
};
SdlImpl *MainWindow::get_sdl_impl() {
  return sdl_impl.get();
};
Canvas *MainWindow::get_canvas() {
  return canvas.get();
};

bool MainWindow::run() {
  bool is_running = true;
  canvas->init(*sdl_impl);

  SDL_Event event;
  while(is_running) {
    canvas->update(*sdl_impl);
    sdl_impl->show();


    int width, height;
    sdl_impl->get_win_size(width, height);
    while(sdl_impl->poll_event(event)) {
      manager->set_state(*sdl_impl, event);

      is_running = manager->handle_action();
      if(!is_running) break;
    }
  }
  return true;
}
