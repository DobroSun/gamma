#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include <memory>

#include "gamma/window.hpp"
#include "gamma/globals.hpp"

int main(int argc, char **argv) {
  if(SDL_Init(SDL_INIT_EVERYTHING)) {
    std::cerr << "Error Initializing SDL: " << SDL_GetError() << std::endl;
    return 1;
  }
  
  std::unique_ptr<Window> window(new GameWindow());
  window->run();
  return 0;
}
