#include "gamma/pch.hpp"

#include "gamma/window.hpp"
#include "gamma/gamma_factory.hpp"

int main(int argc, char **argv) {
  if(SDL_Init(SDL_INIT_EVERYTHING)) {
    std::cerr << "Error Initializing SDL: " << SDL_GetError() << std::endl;
    return 1;
  }
  
  std::unique_ptr<GammaFactory> fact(new MainFactory);
  std::unique_ptr<Window> window(new MainWindow(fact.get()));
  return !window->run();
}
