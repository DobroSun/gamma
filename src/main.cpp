#include "gamma/pch.h"
#include "gamma/init.h"
#include "gamma/input_handler.h"
#include "gamma/utility.h"
#include "gamma/gap_buffer.h"
#include "gamma/view.h"
#include "gamma/update.h"
#include "gamma/buffer.h"




int main(int argc, char **argv) {
  if(Init_SDL()) return 1;
  char *filename = read_args(argc, argv);

  bool success = load_buffer_from_file(filename);
  if(!success) {
    fprintf(stderr, "Error opening file: \"%s\".\n", filename);
    return 1;
  }


  SDL_Window *win = SDL_CreateWindow("Gamma",
                         SDL_WINDOWPOS_CENTERED,
                         SDL_WINDOWPOS_CENTERED,
                         Width, Height,
                         SDL_WINDOW_RESIZABLE);
  SDL_Renderer *renderer = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED); assert(renderer);
  TTF_Font *gfont        = TTF_OpenFont((assets_fonts+courier).c_str(), ptsize);  assert(gfont);
  SDL_SetWindowMinimumSize(win, 300, 300); // @Bug: sets only width == height.
  TTF_SizeText(gfont, "G", &fw, &fh);      // Got size of font in fw, fh.
  assert(fw); assert(fh);


  texture_map alphabet;
  texture_map selected;
  create_alphabet(renderer, gfont, alphabet, selected);


  bool buffer_has_been_changed = true;
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
          buffer_has_been_changed = true;
        } break;

        case SDL_WINDOWEVENT: {
          handle_resize(e, win);
          buffer_has_been_changed = true;
        } break;
      }
    }

    if(buffer_has_been_changed) {
      update(renderer, alphabet, selected);
      buffer_has_been_changed = false;
    }
    SDL_RenderPresent(renderer);

  }

  for(auto &pair: alphabet) {
    SDL_DestroyTexture(pair.second);
  }
  for(auto &pair: selected) {
    SDL_DestroyTexture(pair.second);
  }
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(win);
  TTF_CloseFont(gfont);
  TTF_Quit();
  SDL_Quit();

  return 0;
}
