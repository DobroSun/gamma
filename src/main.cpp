#include "gamma/pch.h"
#include "gamma/init.h"
#include "gamma/input_handler.h"
#include "gamma/utility.h"
#include "gamma/timer.h"
#include "gamma/cursor.h"
#include "gamma/scroll_bar.h"
#include "gamma/gap_buffer.h"
#include "gamma/view.h"




int main(int argc, char **argv) {
  if(Init_SDL()) return 1;
  auto filename =  read_args(argc, argv);
  std::fstream file{filename};

  if(!file) {
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
  Cursor cursor{0, 0};


  // Starts timer to update the cursor.
  SDL_TimerID cursor_timer = StartTimer(300);
  SDL_SetWindowMinimumSize(win, 300, 300); // Bug; sets only width == height.

  int fw, fh;
  TTF_SizeText(gfont, "G", &fw, &fh);
  // Got size of font in fw, fh.


  // Loading file in memory.
  buffer_t buffer;
  LoadFile(buffer, file);


  ScrollBar scroll_bar{buffer.size()};
  ScrollBar* active_bar = nullptr;


  std::unordered_map<char, SDL_Texture *> alphabet;
  std::unordered_map<char, SDL_Texture *> selected;
  create_alphabet(renderer, gfont, alphabet, selected);


  buffer_view b_view(buffer, 0);
  bool done = false;
  while(!done) {

    SDL_Event e;
    if(SDL_PollEvent(&e)) {
      switch(e.type) {
        case SDL_QUIT: {
          done = true;
        } break;

        case SDL_MOUSEBUTTONDOWN: {
          handle_mousebuttondown(e, cursor, scroll_bar, fw, active_bar);
        } break;

        case SDL_MOUSEBUTTONUP: {
          handle_mousebuttonup(e, active_bar);
        } break;

        case SDL_MOUSEMOTION: {
          handle_mousemotion(e, b_view, active_bar);
        } break;

        case SDL_KEYDOWN: {
          handle_keydown(e, b_view, cursor, done);
        } break;

        case SDL_MOUSEWHEEL: {
          handle_mousewheel(e, b_view, scroll_bar, cursor);
        } break;

        case SDL_WINDOWEVENT: {
          handle_resize(e, win, scroll_bar, b_view);
        } break;
      }
    }
    fix_cursor(b_view, cursor);


    // Background color.
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); 
    SDL_RenderClear(renderer);


    // Update window.
    int tw = 0, th = 0;
    for(int i = 0; i < numrows(); i++) {
      auto string = b_view.at_or(i, gap_buffer<char>{});
      for(unsigned j = 0; j < string.size(); j++) {
        auto char_texture = alphabet[string[j]];
        assert(char_texture);

        SDL_QueryTexture(char_texture, nullptr, nullptr, &tw, &th);
        SDL_Rect dst {TextLeftBound+j*fw, TextUpperBound+i*fsize, tw, th};
        SDL_RenderCopy(renderer, char_texture, nullptr, &dst);
      }
    }
    /*
    auto c = b_view.at_or(cursor.i, "").at_or(cursor.j, ' ');
    auto cursor_texture = selected[c];
    timer::update_cursor(renderer, cursor_texture, cursor, fw);
    */

    draw_bar(scroll_bar, renderer);
    SDL_RenderPresent(renderer);
  }
  
/*
  SDL_RemoveTimer(cursor_timer);
  SDL_DestroyTexture(cursor_texture);
  for(auto &txt: textures) {
    SDL_DestroyTexture(txt);
  }
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(win);
  TTF_CloseFont(gfont);
  TTF_Quit();
  SDL_Quit();
*/
  return 0;
}
