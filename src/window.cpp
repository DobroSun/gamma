#include "gamma/window.hpp"
#include "gamma/globals.hpp"

#include <SDL2/SDL.h>
#include <iostream>
#include <memory>


ExitWindow::ExitWindow() {
  sdl_win = SDL_CreateWindow("Are you sure want to exit?",
                  SDL_WINDOWPOS_CENTERED,
                  SDL_WINDOWPOS_CENTERED,
                  400, 200, 0);
}

ExitWindow::~ExitWindow() {
  SDL_DestroyWindow(sdl_win);
}


bool ExitWindow::run() {
  SDL_Event event;
  while(1) {
    while(SDL_PollEvent(&event)) {
      if(event.type == SDL_WINDOWEVENT
         && event.window.event == SDL_WINDOWEVENT_CLOSE) {
          return false;
      }
    }
  } 
}


GameWindow::GameWindow() {
  is_running = true;
  sdl_win = SDL_CreateWindow("",
                  SDL_WINDOWPOS_CENTERED,
                  SDL_WINDOWPOS_CENTERED,
                  WIDTH, HEIGHT, 0);
}

GameWindow::~GameWindow() {
  SDL_DestroyWindow(sdl_win);
}

bool GameWindow::run() {
  SDL_SetWindowResizable(sdl_win,
                        SDL_TRUE);
  
  SDL_Event event;
  while(is_running) {
    while(SDL_PollEvent(&event)) {
      switch(event.type) {
        case SDL_QUIT:
          std::unique_ptr<Window> ExWindow(new ExitWindow());
          is_running = ExWindow->run();
          break;
      }
    }
  }
  return true;
}
