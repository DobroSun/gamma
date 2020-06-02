#ifndef GAMMA_INIT_H
#define GAMMA_INIT_H
int Init_SDL() {
  if(SDL_Init(SDL_INIT_EVENTS | SDL_INIT_TIMER)) {
    std::cerr << "Error Initializing SDL: " << SDL_GetError() << std::endl;
    return 1;
  }
  if(TTF_Init()) {
    std::cerr << "Error Initializing TTF: " << TTF_GetError() << std::endl;
    return 1;
  }
  return 0;
}

std::string read_args(int argc, char **argv) {
  return (argc < 2)? "": argv[1];
}

#endif
