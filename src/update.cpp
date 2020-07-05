#include "gamma/pch.h"
#include "gamma/update.h"
#include "gamma/globals.h"
#include "gamma/view.h"
#include "gamma/scroll_bar.h"
#include "gamma/utility.h"
#include "gamma/gap_buffer.h"
#include "gamma/timer.h"


static int tw = 0, th = 0;
void update(SDL_Renderer *renderer, const buffer_view &b_view, const ScrollBar &scroll_bar, std::unordered_map<char, SDL_Texture *> &alphabet, std::unordered_map<char, SDL_Texture *> &selected) {
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


      // Update cursored symbol.
      if(line == cursor.i && (int)j == cursor.j) {
        auto cursor_texture = selected[c];
        update_cursor(renderer, cursor_texture, i_pixels, j_pixels);
      }
    }
  }
}
