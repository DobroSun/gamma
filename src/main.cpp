#include "gamma/pch.h"
#include "gamma/init.h"
#include "gamma/input.h"
#include "gamma/buffer.h"
#include "gamma/font.h"
#include "gamma/console.h"


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
    auto begin = std::chrono::steady_clock::now();
    defer {
      auto end = std::chrono::steady_clock::now();
      double cur_time = std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count();
      unsigned to_sleep = 1000000/60 - cur_time;
      sleep(to_sleep);
      if(show_fps) {
        end = std::chrono::steady_clock::now();
        cur_time = std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count();
        char fps[64];
        sprintf(fps, "******* FPS is: %lf *******", 1000000 / cur_time);
        print(fps);
      }
    };
    //printFPS;
    process_input();
    update();
  }
  return 0;
}
