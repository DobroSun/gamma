#include "gamma/pch.h"
#include "gamma/init.h"
#include "gamma/input_handler.h"
#include "gamma/utility.h"
#include "gamma/timer.h"
#include "gamma/gap_buffer.h"
#include "gamma/view.h"
#include "gamma/update.h"
#include "gamma/buffer.h"



int main(int argc, char **argv) {
  if(Init_SDL()) return 1;
  string filename = read_args(argc, argv);


  // Loading file in memory.
  bool success = load_buffer_from_file(filename);
  if(!success) {
    fprintf(stderr, "Error opening file: %s", filename.data());
    return 1;
  }



  SDL_Window *win = SDL_CreateWindow("Gamma",
                         SDL_WINDOWPOS_CENTERED,
                         SDL_WINDOWPOS_CENTERED,
                         Width, Height,
                         SDL_WINDOW_RESIZABLE);
  auto renderer = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED); assert(renderer);
  auto gfont = TTF_OpenFont((assets_fonts+courier).c_str(), ptsize);     assert(gfont);

  // Starts timer to update the cursor.
  SDL_TimerID cursor_timer = StartTimer(300);
  (void)cursor_timer;
  SDL_SetWindowMinimumSize(win, 300, 300); // Bug; sets only width == height.


  // Got size of font in fw, fh.
  TTF_SizeText(gfont, "G", &fw, &fh);
  assert(fw); assert(fh);


  texture_map alphabet;
  texture_map selected;
  create_alphabet(renderer, gfont, alphabet, selected);


  bool done = false;
  while(!done) {
    SDL_Event e;
    if(SDL_PollEvent(&e)) {
      switch(e.type) {
        case SDL_QUIT: {
          done = true;
        } break;

        case SDL_KEYDOWN: {
          handle_keydown(e, done);
        } break;

        case SDL_WINDOWEVENT: {
          handle_resize(e, win);
        } break;
      }
    }

    update(renderer, alphabet, selected);
    SDL_RenderPresent(renderer);
  }
  
  SDL_RemoveTimer(cursor_timer);
  for(auto &[key, texture]: alphabet) {
    SDL_DestroyTexture(texture);
    (void)key;
  }
  for(auto &[key, texture]: selected) {
    SDL_DestroyTexture(texture);
    (void)key;
  }
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(win);
  TTF_CloseFont(gfont);
  TTF_Quit();
  SDL_Quit();
  return 0;
}
