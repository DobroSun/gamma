#include "gamma/pch.h"
#include "gamma/input_handler.h"
#include "gamma/globals.h"
#include "gamma/scroll_bar.h"
#include "gamma/cursor.h"
#include "gamma/gap_buffer.h"
#include "gamma/view.h"


static int offset = 0; // Needs to be inside buffer_view as another data member.



static bool in_buffer(double x, double y) {
  return y >= TextUpperBound && x >= TextLeftBound && y < Height - TextBottomBound;
}

static bool is_last_line(int index, int start, int cur_length) {
  return index-start >= numrows()-1-offset-cur_length;
}


void cursor_down_detail(buffer_view &buffer, bool (*last_line)(int, int, int)) {
  auto &start = buffer.start;
  auto &i = buffer.cursor.i;

  const int max_size = buffer.size()-1;

  if(i == max_size) {
    // On the last line of file.
    return;
  }

  const int max_line = buffer_width() / fw;
  auto first_line_arity = buffer[start].size()/max_line;
  auto current_line_arity = buffer[i].size()/max_line;
	auto next_line_arity = buffer[i+1].size()/max_line;

  if(!last_line(i, start, current_line_arity)) {
   // Just go to the next line inside buffer.
   // so start and offset don't change.

  } else {
    int diff = next_line_arity - first_line_arity;

    if(diff > 0) {
      while(diff > 0) { // @Wrong. @Incomplete.
        start++;
        offset++;
        first_line_arity = buffer[start].size()/max_line;
        diff -= first_line_arity + 1;
      }
      start++;

    } else if(diff < 0) {
      int count = 1;
      while(diff < 0) {
        start++;
        count++;
        next_line_arity = buffer[i+count].size()/max_line;
        diff += next_line_arity + 1;
      }

    } else {
      assert(next_line_arity == first_line_arity);
      offset += next_line_arity;
      start += next_line_arity + 1;
    }
  }

  buffer.move_right();
  i++;
  fix_gap(buffer);
}



static void cursor_down(buffer_view &buffer) {
  cursor_down_detail(buffer, is_last_line);
}


static void cursor_up(buffer_view &buffer) {
}

static void cursor_right(buffer_view &buffer) {
  auto &cursor = buffer.cursor;
  auto &i = cursor.i; auto &j = cursor.j;
  auto &buffer_i = buffer[i];

  if(j < (int)buffer_i.size()-1) {
    buffer_i.move_right();
    j++;
  }
}

static void cursor_left(buffer_view &buffer) {
  auto &cursor = buffer.cursor;
  auto &i = cursor.i; auto &j = cursor.j;
  auto &buffer_i = buffer[i];

  if(j > (int)0) {
    buffer_i.move_left();
    j--;
  }
}


bool LoadFile(buffer_t &buffer, const std::string &filename) {
  std::fstream file{filename};
  if(!file) {
    return false;
  }

  std::string input; 
  while(std::getline(file, input)) {
    gap_buffer<char> g;

    for(unsigned i = 0; i < input.size(); i++) {
      g.insert(input[i]);
    }

    g.insert(' ');
    buffer.insert(g);
  }
  return true;
}


void handle_scroll_up(buffer_view &buffer) {
  auto &start = buffer.start;
  if(start == 0) return;

  auto &i = buffer.cursor.i;
  int diff = numrows()-i-1;

  i += (diff < scroll_speed)? diff: scroll_speed;

  auto start_change = (start < scroll_speed)? start: scroll_speed;
  buffer.decrease_start_by(start_change);
}

void handle_scroll_down(buffer_view &buffer) {
  auto &start = buffer.start;
  auto &i = buffer.cursor.i;

  i -= (i < scroll_speed)? i: scroll_speed;

  unsigned total = buffer.size()-1; int ts = total-start;
  int speed = (ts < scroll_speed)? ts: scroll_speed;
  auto start_change = (start == total)? 0: speed;

  buffer.increase_start_by(start_change);
}


void handle_mousewheel(const SDL_Event &e, buffer_view &buffer, ScrollBar &bar) {
  auto &wheel = e.wheel;
  auto &start = buffer.start;

  if(wheel.y > 0) {
    handle_scroll_up(buffer);
    start_to_bar(buffer, bar);

    if(start == 0) return;

  } else if(wheel.y < 0) {
    handle_scroll_down(buffer);
    start_to_bar(buffer, bar);
  }
}


void handle_resize(const SDL_Event &e, SDL_Window *win, ScrollBar &bar, const buffer_view &buffer) {
  if(e.window.event == SDL_WINDOWEVENT_RESIZED) {
    SDL_GetWindowSize(win, &Width, &Height);
    reinit_bar(bar, buffer);
  }
}


void handle_keydown(const SDL_Event &e, buffer_view &buffer, bool &done) {
  auto keysym = e.key.keysym;
  auto key = keysym.sym;

  auto &cursor = buffer.cursor;
  auto &i = cursor.i; auto &j = cursor.j;
  if(key == SDLK_LSHIFT || key == SDLK_RSHIFT) {
    SDL_SetModState((SDL_Keymod)KMOD_SHIFT);
    return;

  } else if(key == SDLK_ESCAPE) {
    done = true;
    return;

  } else if(key == SDLK_BACKSPACE) {
    if(buffer[i].pre_len > 0) {
      j--;
      buffer[i].backspace();
    } else {
      buffer[i].backspace();
    }

  } else if(key == SDLK_DELETE) {
    buffer[i].del();
    return;

  } else if(key == SDLK_RETURN) {
    gap_buffer<char> to_end;
    gap_buffer<char> from_start;

    auto buf_i = buffer[i];
    for(auto k = 0; k < j; k++) {
      from_start.insert(buf_i[k]);
    }
    from_start.insert(' '); // add extra space.
    for(unsigned k = j; k < buf_i.size(); k++) {
      to_end.insert(buf_i[k]);
    }
    buffer.add(from_start);
    

    if(i < numrows()-1) {
      buffer[++i] = to_end;

    } else {
      buffer.start++;
      buffer[i] = to_end;
    }
    j = 0;

    return;

  } else if(key == SDLK_UP) {
    cursor_up(buffer);
    return;

  } else if(key == SDLK_DOWN) {
    cursor_down(buffer);
    return;

  } else if(key == SDLK_LEFT) {
    cursor_left(buffer);
    return;

  } else if(key == SDLK_RIGHT) {
    cursor_right(buffer);
    return;
  }


  auto mod = keysym.mod;
  bool is_shift = mod == KMOD_SHIFT;
  auto shifted = slice(key_lookup, underlying);


  // @Note: Need to load settings from 
  // file and generate keyscodes.
  for_each(key_lookup) {
    if(key == *it) {
      char push;
      if(is_shift && isalpha(key)) {
        push = toupper(key);

      } else if(is_shift && in(shifted, key)) {
        const SDL_Keycode *sh = it;
        sh += key_offset;
        push = (char)*sh;

      } else {
        push = (char)key;
      }

      buffer[i].add(push);
      j++;
      return;
    }
  }
}


void handle_mousemotion(const SDL_Event &e, buffer_view &buffer, ScrollBar*& active) {
  auto &motion = e.motion;
  auto &x = motion.x; auto &y = motion.y;
  (void)x;

  if(active) {
    auto yy = y-TextUpperBound;
    if(yy <= active->h) {
      bar_to_start(buffer, active, yy);
    }
  }
}


void handle_mousebuttondown(const SDL_Event &e, ScrollBar &bar, ScrollBar*& active) {
  auto &button = e.button;
  auto &b_type = button.button; 
  auto &b_click = button.clicks;
  auto x = button.x; auto y = button.y;

  if(clicked_small(bar, x, y)) {
    got_clicked(bar, x, y);
    active = &bar;

  } else if(clicked_bar(bar, x, y)) {

  } else if(in_buffer(x, y)) {
    if(b_type == SDL_BUTTON_LEFT && b_click == 3) {
      std::cout << "Triple click!" << std::endl;

    } else if(b_type == SDL_BUTTON_LEFT && b_click == 2) {
      std::cout << "Double click!" << std::endl;

    } else if(b_type == SDL_BUTTON_LEFT) {
      std::cout << "Click!" << std::endl;
    }
  }
}


void handle_mousebuttonup(const SDL_Event &e, ScrollBar*& active) {
  auto &button = e.button;
  if(button.button == SDL_BUTTON_LEFT) {
    active = nullptr;
  }
}
