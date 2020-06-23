#include "gamma/pch.h"
#include "gamma/input_handler.h"
#include "gamma/globals.h"
#include "gamma/scroll_bar.h"
#include "gamma/cursor.h"
#include "gamma/gap_buffer.h"
#include "gamma/view.h"



static bool in_buffer(double x, double y) {
  return y >= TextUpperBound && x >= TextLeftBound && y < Height - TextBottomBound;
}

static bool is_last_line(int index, int start, int length_on_screen) {
  return index+start == numrows()-length_on_screen;
}

// 
// Function moves cursor to the next line. If cursor is on the last line inside screen,
// It just moves to next line, but if not, it looks on the last line and goes down by 
// length of this line on screen. However, if the first line on screen also is `huge` it 
// it will go down by length that is needed to keep all lines on screen `full`.
// 
static void next_line(buffer_view &buffer, int fw) {
  auto &start = buffer.start;
  auto &cursor = buffer.cursor;
  auto &i = cursor.i; auto &j = cursor.j;

  int max_size = buffer.size()-1;
  int max_line = (Width-TextLeftBound-25)/fw;

  if(i+start == max_size) {
    // On the last line of file.
    return;
  }

  std::cout << buffer.pre_len() << std::endl;

  auto length_on_screen = buffer[i+1].size()/max_line + 1;
  auto current_length = buffer[i].size()/max_line + 1;
  if(!is_last_line(i, start, current_length)) {
    // Just go to the next line inside buffer.
    buffer.move_right();
    i++;

  } else {

    //start += length_on_screen;
    buffer.move_right();
    i++;
  }
  std::cout << i << " and " << buffer[i][0] << std::endl;
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


void handle_keydown(const SDL_Event &e, buffer_view &buffer, bool &done, int fw) {
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
    move_cursor(buffer, i, j, i-1, j);
    return;

  } else if(key == SDLK_DOWN) {
    next_line(buffer, fw);
    return;

  } else if(key == SDLK_LEFT) {
    move_cursor(buffer, i, j, i, j-1);
    return;

  } else if(key == SDLK_RIGHT) {
    move_cursor(buffer, i, j, i, j+1);
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


void handle_mousebuttondown(const SDL_Event &e, ScrollBar &bar, int fw, ScrollBar*& active) {
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
