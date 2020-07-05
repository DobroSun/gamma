#include "gamma/pch.h"
#include "gamma/init.h"
#include "gamma/input_handler.h"
#include "gamma/utility.h"
#include "gamma/timer.h"
#include "gamma/cursor.h"
#include "gamma/scroll_bar.h"
#include "gamma/gap_buffer.h"
#include "gamma/view.h"
#include "gamma/update.h"





int main(int argc, char **argv) {
  if(Init_SDL()) return 1;
  auto filename = read_args(argc, argv);


  // Loading file in memory.
  buffer_t buffer;
  bool success = LoadFile(buffer, filename);
  if(!success) {
    std::cerr << "Error opening file: " << filename << std::endl;
    return 1;
  }



  SDL_Window *win = SDL_CreateWindow("Gamma",
                         SDL_WINDOWPOS_CENTERED,
                         SDL_WINDOWPOS_CENTERED,
                         Width, Height,
                         SDL_WINDOW_RESIZABLE);
  auto renderer = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
  auto gfont = TTF_OpenFont((assets_fonts+courier).c_str(), ptsize);


  // Starts timer to update the cursor.
  SDL_TimerID cursor_timer = StartTimer(300);
  (void)cursor_timer;
  SDL_SetWindowMinimumSize(win, 300, 300); // Bug; sets only width == height.


  // Got size of font in fw, fh.
  TTF_SizeText(gfont, "G", &fw, &fh);
  assert(fw); assert(fh);



  ScrollBar scroll_bar{buffer.size()};
  ScrollBar* active_bar = nullptr;



  std::unordered_map<char, SDL_Texture *> alphabet;
  std::unordered_map<char, SDL_Texture *> selected;
  create_alphabet(renderer, gfont, alphabet, selected);


  buffer_view b_view{buffer};
  bool done = false;
  while(!done) {
    SDL_Event e;
    if(SDL_PollEvent(&e)) {
      switch(e.type) {
        case SDL_QUIT: {
          done = true;
        } break;

        case SDL_MOUSEBUTTONDOWN: {
          handle_mousebuttondown(e, b_view, scroll_bar, active_bar);
        } break;

        case SDL_MOUSEBUTTONUP: {
          handle_mousebuttonup(e, active_bar);
        } break;

        case SDL_MOUSEMOTION: {
          handle_mousemotion(e, b_view, active_bar);
        } break;

        case SDL_KEYDOWN: {
          handle_keydown(e, b_view, done);
        } break;

        case SDL_MOUSEWHEEL: {
          handle_mousewheel(e, b_view, scroll_bar);
        } break;

        case SDL_WINDOWEVENT: {
          handle_resize(e, win, scroll_bar, b_view);
        } break;
      }
    }

    // Background color.
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); 
    SDL_RenderClear(renderer);

    update(renderer, b_view, scroll_bar, alphabet, selected);

    draw_bar(scroll_bar, renderer);
    SDL_RenderPresent(renderer);
  }
  
/*
  SDL_RemoveTimer(cursor_timer);
  for(auto &[key, texture]: alphabet) {
    SDL_DestroyTexture(texture);
  }
  for(auto &[key, texture]: selected) {
    SDL_DestroyTexture(texture);
  }
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(win);
  TTF_CloseFont(gfont);
  TTF_Quit();
  SDL_Quit();
*/
  return 0;
}
