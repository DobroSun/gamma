#include "gamma/pch.h"
#include "gamma/update.h"
#include "gamma/globals.h"
#include "gamma/view.h"
#include "gamma/scroll_bar.h"
#include "gamma/utility.h"
#include "gamma/gap_buffer.h"
#include "gamma/timer.h"


static int tw = 0, th = 0;
static bool is_end(int i, int offset) {
  if(numrows() - i - offset) {
    return false;
  }
  return true;
}


// @Note: Originally I have representation 1 index -- 1 line.
// But with big lines, this won't work.
// So I have to explicitly break from loop if there is no
// place to draw new line.
void update(SDL_Renderer *renderer, const buffer_view &b_view, const ScrollBar &scroll_bar, std::unordered_map<char, SDL_Texture *> &alphabet, std::unordered_map<char, SDL_Texture *> &selected, int fw) {
  unsigned max_line = (Width-TextLeftBound-scroll_bar.w)/fw;
  char nrows = numrows();
  auto &cursor = b_view.cursor;

  for(int i = 0, offset_y = 0; i < nrows; i++) {
    const auto &string = b_view[i];


    for(unsigned j = 0, offset_x = 0; j < string.size(); j++) {
      auto char_texture = alphabet[string[j]];
      assert(char_texture);


      if(j-offset_x*max_line == max_line) {
        offset_x++;
        offset_y++;
      }

      if(is_end(i, offset_y)) {
        break;
      }
      
      int j_relative = (j - offset_x*max_line) * fw;
      int i_relative = (i+offset_y) * fsize;

      // Update character.
      SDL_QueryTexture(char_texture, nullptr, nullptr, &tw, &th);
      SDL_Rect dst {(int)(TextLeftBound+(j-offset_x*max_line)*fw), TextUpperBound+(i+offset_y)*fsize, tw, th};
      SDL_RenderCopy(renderer, char_texture, nullptr, &dst);


      // If char is cursor, update cursor.
      // @Temporary: When array of offsets will be done.
      // This must be updated after update method.
      if(i == cursor.i && (int)j == cursor.j) {
        char c = b_view[cursor.i][cursor.j];
        auto cursor_texture = selected[c];
        update_cursor(renderer, cursor_texture, i_relative, j_relative);
      }
    }

    if(is_end(i, offset_y)) {
      break;
    }
  }
}
