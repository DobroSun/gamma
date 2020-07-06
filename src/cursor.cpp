#include "gamma/pch.h"
#include "gamma/cursor.h"
#include "gamma/globals.h"
#include "gamma/utility.h"
#include "gamma/view.h"
#include "gamma/gap_buffer.h"


void fix_gap(buffer_view &buffer) {
  auto &cursor = buffer.cursor;
  auto &line = buffer[cursor.i];
  int diff = cursor.j - line.pre_len;
  if(diff > 0) {
    // cursor.j is bigger than start of gap.
    // so moving gap right.
    line.move_right_by(diff);
  
  } else if(diff < 0) {
    // cursor.j is less than start of gap.
    line.move_left_by(-diff);

  } else {
    assert(!diff);
    // There is no difference between cursor.j and start index of gap.
    // do nothing.
  } 
}

// Copy&Paste from update.cpp
/*
static bool end(int i, int offset) {
  if(numrows() - i - offset) {
    return false;
  }
  return true;
}

void get_pos(buffer_view &buffer, int x, int y) {
  // Precompute offset.
  // Copy&Paste from update.cpp
  unsigned max_line = (Width-TextLeftBound-25)/fw; // where 25 is scroll_bar.width.
  char nrows = numrows();
  int offset_y = 0;
  for(int line = buffer.start, i = 0; i < nrows; i++, line++) {
    const auto &string = buffer[line];

    for(unsigned j = 0, offset_x = 0; j < string.size(); j++) {
      if(j-offset_x*max_line == max_line) {
        offset_y++;
        offset_x++;
      }

      // Here is real get_function.
      int clicked_pos_i = (y-TextUpperBound)/fsize - offset_y;
      if(clicked_pos_i == i) {
        int clicked_pos_j = (x-TextLeftBound)/fw + offset_x*max_line;
        // @Wrong:
        // Need to also move gap.
        buffer.cursor = {clicked_pos_i, clicked_pos_j};
        return;
      }
      // ;
      

      if(end(i, offset_y)) {
        break;
      }
    }
    if(end(i, offset_y)) {
      break;
    }
  }
}
*/
