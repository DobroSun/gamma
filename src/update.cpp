#include "gamma/pch.h"
#include "gamma/update.h"
#include "gamma/globals.h"
#include "gamma/view.h"
#include "gamma/utility.h"
#include "gamma/buffer.h"
#include "gamma/input_handler.h"
#include "gamma/timer.h"


static int tw = 0, th = 0;
static void update_editor(SDL_Renderer *renderer, texture_map &alphabet, texture_map &selected) {
  // Set background color.
  SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); 
  SDL_RenderClear(renderer);

  const auto &b_view = get_buffer();
  auto &cursor = b_view.cursor;
  const unsigned offset = b_view.start_j;
  const unsigned max_size = buffer_width() / fw;

  for(int line = b_view.start, i = 0; i < numrows(); i++, line++) {
    auto string = b_view.get_view(line);

    for(unsigned j = offset*max_size; j < string.size(); j++) {
      char c = string[j];
      auto char_texture = alphabet[c];
      assert(char_texture);

      int j_pixels = (j - offset*max_size) * fw ;
      int i_pixels = i * fsize;

      // Update character.
      SDL_QueryTexture(char_texture, nullptr, nullptr, &tw, &th);
      SDL_Rect dst {TextLeftBound+j_pixels, TextUpperBound+i_pixels, tw, th};
      SDL_RenderCopy(renderer, char_texture, nullptr, &dst);


      // Update selected symbol.
      if(line == cursor.i && (int)j == cursor.j) {
        auto cursor_texture = selected[c];
        update_cursor(renderer, cursor_texture, i_pixels, j_pixels);
      }
    }
  }
}

static void update_console(SDL_Renderer *renderer, texture_map &alphabet, texture_map &selected) {
  // Nothing for now.
  (void)renderer;
  (void)alphabet;
  (void)selected;
}

void update(SDL_Renderer *renderer, texture_map &alphabet, texture_map &selected) {
  switch(get_editor_mode()) {
    case Editor: {
      update_editor(renderer, alphabet, selected);
    } break;

    case Console: {
      update_console(renderer, alphabet, selected);
    } break;
  }
}
