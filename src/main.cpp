#include "gamma/pch.h"
#include "gamma/init.h"
#include "gamma/input.h"
#include "gamma/buffer.h"
#include "gamma/font.h"


#define destruct \
  defer { \
    for(auto &pair: get_alphabet()) { \
      SDL_DestroyTexture(pair.second); \
    } \
    SDL_DestroyRenderer(get_renderer()); \
    SDL_DestroyWindow(get_win()); \
    TTF_CloseFont(get_font()); \
    TTF_Quit(); \
    SDL_Quit(); \
  }

int main(int argc, char **argv) {
  if(Init_SDL()) return 1;
  init(argc, argv);

  destruct;

  while(!should_quit) {
    //printFPS;
    process_input();
    update();
  }
  return 0;
}
