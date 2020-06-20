#include "gamma/pch.h"
#include "gamma/input_handler.h"
#include "gamma/globals.h"
#include "gamma/scroll_bar.h"
#include "gamma/cursor.h"
#include "gamma/gap_buffer.h"
#include "gamma/view.h"


void LoadFile(buffer_t &buffer, std::fstream &file) {
  std::string input; 
  while(std::getline(file, input)) {
    gap_buffer<char> g;

    for(unsigned i = 0; i < input.size(); i++) {
      g.insert(input[i]);
    }
    g.insert(' ');

    buffer.insert(g);
  }
}


static bool in_buffer(double x, double y) {
  return y >= TextUpperBound && x >= TextLeftBound && y < Height - TextBottomBound;
}


void handle_scroll_up(buffer_view &buffer, Cursor &cursor) {
  auto &start = buffer.start;
  if(start == 0) return;

  auto &i = cursor.i;
  int diff = numrows()-i-1;

  i += (diff < scroll_speed)? diff: scroll_speed;

  auto start_change = (start < scroll_speed)? start: scroll_speed;
  buffer.decrease_start_by(start_change);
}

void handle_scroll_down(buffer_view &buffer, Cursor &cursor) {
  auto &start = buffer.start;
  auto &i = cursor.i;

  i -= (i < scroll_speed)? i: scroll_speed;

  unsigned total = buffer.size()-1; int ts = total-start;
  int speed = (ts < scroll_speed)? ts: scroll_speed;
  auto start_change = (start == total)? 0: speed;

  buffer.increase_start_by(start_change);
}


void handle_mousewheel(const SDL_Event &e, buffer_view &buffer, ScrollBar &bar, Cursor &cursor) {
  auto &wheel = e.wheel;
  auto &start = buffer.start;

  if(wheel.y > 0) {
    handle_scroll_up(buffer, cursor);
    start_to_bar(buffer, bar);

    if(start == 0) return;

  } else if(wheel.y < 0) {
    handle_scroll_down(buffer, cursor);
    start_to_bar(buffer, bar);
  }
}


void handle_resize(const SDL_Event &e, SDL_Window *win, ScrollBar &bar, const buffer_view &buffer) {
  if(e.window.event == SDL_WINDOWEVENT_RESIZED) {
    SDL_GetWindowSize(win, &Width, &Height);
    reinit_bar(bar, buffer);
  }
}


void handle_keydown(const SDL_Event &e, buffer_view &buffer, Cursor &cursor, bool &done) {
  auto keysym = e.key.keysym;
  auto key = keysym.sym;

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
    } 
    buffer[i].backspace();

    return;

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
      // Cursor places on last line.
      // It stays here and start increases.

      buffer.start++;
      buffer[i] = to_end;
    }
    j = 0;

    return;

  } else if(key == SDLK_UP) {
    move_cursor(buffer, i, j, i-1, j);
    return;

  } else if(key == SDLK_DOWN) {
    move_cursor(buffer, i, j, i+1, j);
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


void handle_mousebuttondown(const SDL_Event &e, Cursor &cursor, ScrollBar &bar, int fw, ScrollBar*& active) {
  auto &button = e.button;
  auto &b_type = button.button; 
  auto &b_click = button.clicks;
  auto x = button.x; auto y = button.y;

  if(clicked_small(bar, x, y)) {
    got_clicked(bar, x, y);
    active = &bar;

  } else if(clicked_bar(bar, x, y)) {

  } else if(in_buffer(x, y)) {
    std::cout << "Buffer clicked" << std::endl;

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

