#include "gamma/pch.h"
#include "gamma/init.h"
#include "gamma/input_handler.h"
#include "gamma/utility.h"
#include "gamma/timer.h"
#include "gamma/cursor.h"
#include "gamma/view.h"
#include "gamma/scroll_bar.h"




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
  std::vector<SDL_Texture *> textures;
  String buffer;
  Cursor cursor{0, 0};


  // Starts timer to update the cursor.
  SDL_TimerID cursor_timer = StartTimer(300);
  (void)cursor_timer;
  SDL_SetWindowMinimumSize(win, 300, 300); // Bug; sets only width == height.

  int fw, fh;
  TTF_SizeText(gfont, "G", &fw, &fh);
  // Got size of font in fw, fh.
  


  // Loading file in memory.
  LoadFile(renderer, gfont, buffer, textures, file);


  ScrollBar scroll_bar{buffer.size()};
  SDL_Texture *cursor_texture = init_cursor(renderer, gfont, buffer, cursor);
  ScrollBar* active_bar = nullptr;


  Uint32 start = 0;
  bool done = false;
  while(!done) {
    textures_view t_view(textures, start);
    buffer_view b_view(buffer, start, 0);

    SDL_Event e;
    if(SDL_PollEvent(&e)) {
      switch(e.type) {
        case SDL_QUIT: {
          done = true;
        } break;

        case SDL_MOUSEBUTTONDOWN: {
          handle_mousebuttondown(e, cursor, b_view, scroll_bar, fw, active_bar);
        } break;

        case SDL_MOUSEBUTTONUP: {
          handle_mousebuttonup(e, active_bar);
        } break;

        case SDL_MOUSEMOTION: {
          handle_mousemotion(e, b_view, active_bar, start);
        } break;

        case SDL_KEYDOWN: {
          handle_keydown(e, done);
        } break;

        case SDL_MOUSEWHEEL: {
          handle_mousewheel(e, b_view, scroll_bar, cursor, start);
        } break;

        case SDL_WINDOWEVENT: {
          handle_resize(e, win, scroll_bar, b_view);
        } break;
      }
    }
    /*
    // FIXME, Please: Cursor is broken.
    fix_cursor(b_view, cursor);
    cursor_texture = render_cursor(renderer, gfont, cursor_texture, b_view, cursor);
    */


    // Background color.
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); 
    SDL_RenderClear(renderer);


    // Update window.
    for(int i = 0; i < numrows(); i++) {
      auto txt = t_view.at_or(i, nullptr);

      int tw = 0, th = 0;
      SDL_QueryTexture(txt, nullptr, nullptr, &tw, &th);
      SDL_Rect dst {TextLeftBound, TextUpperBound+i*fsize, tw, th};
      SDL_RenderCopy(renderer, txt, nullptr, &dst);

    }
    timer::update_cursor(renderer, cursor_texture, cursor, fw);

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
