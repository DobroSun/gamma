#include "gamma/pch.h"
#include "gamma/cursor.h"
#include "gamma/globals.h"
#include "gamma/utility.h"
#include "gamma/view.h"
#include "gamma/gap_buffer.h"


bool operator==(const Cursor &c1, const Cursor &c2) {
  return c1.i == c2.i && c1.j == c2.j;
}
bool operator!=(const Cursor &c1, const Cursor &c2) {
  return c1.i != c2.i || c1.j != c2.j;
}
/*
void fix_cursor(const buffer_view &buffer, Cursor &c) {
  int i = c.i; int &j = c.j;
  int actual_size = buffer[i].size();

  j = (j > actual_size)? actual_size: j;
}
*/

void move_cursor(buffer_view &buffer, int &from_i, int &from_j, int to_i, int to_j) {
  int max_size = buffer.size()-1;
  auto &start = buffer.start;


  int is_up = to_i+start;
  if(is_up < 0) {
    // If start is zero it won't go up.
    assert(start == 0);
    to_i = 0;

  } else if((unsigned)is_up < start) {
    // If to_i < 0, and start != 0; so we will go up.
    assert(to_i < 0 && start != 0);

    buffer.decrease_start_by(start-is_up);
    return;

  } else if(is_up > max_size) {
    to_i = max_size-start;
  }

  int page_diff = to_i-numrows()+1;
  if(page_diff > 0) {
    // If moving down from the visible part of window.
    // Need to increase buffer.start also.
    buffer.increase_start_by(page_diff);
    return;
  }

  if(to_j < 0) {
    to_j = 0;
  }
  int dest_size = buffer[to_i].size()-1;
  if(to_j > dest_size) {
    to_j = dest_size;
  }
  

  if(from_j == to_j) {
    int diff = to_i - from_i;

    if(diff > 0) {
      for(int k = 0; k < diff; k++) {
        from_i++;
        buffer.move_right();
      }
    } else {
      for(int k = 0; k > diff; k--) {
        from_i--;
        buffer.move_left();
      }
    }

  } else if(from_i == to_i) {
    int diff = to_j - from_j;

    if(diff > 0) {
      for(int k = 0; k < diff; k++) {
        from_j++;
        buffer[to_i].move_right();
      }
    } else {
      for(int k = 0; k > diff; k--) {
        from_j--;
        buffer[to_i].move_left();
      }
    }

  } else {
    // When moving to arbitrary (x, y).

  }
}

