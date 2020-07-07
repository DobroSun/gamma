#include "gamma/pch.h"
#include "gamma/input_handler.h"
#include "gamma/globals.h"
#include "gamma/scroll_bar.h"
#include "gamma/cursor.h"
#include "gamma/gap_buffer.h"
#include "gamma/view.h"


//static EditorMode Mode = Editor;
static bool in_buffer(double x, double y) {
  return y >= TextUpperBound && x >= TextLeftBound && y < Height - TextBottomBound;
}

static void cursor_right_detail(buffer_view &buffer) {
  auto &cursor = buffer.cursor;
  auto &start_j = buffer.start_j;
  auto &i = cursor.i; auto &j = cursor.j;
  auto &buffer_i = buffer[i];

  const int max_line = (start_j+1) * buffer_width() / fw;
  if(j == max_line-1) {
    start_j++;
  }

  if(j < (int)buffer_i.size()-1) {
    buffer_i.move_right();
    j++;
  }
}

static void cursor_right(buffer_view &buffer) {
  cursor_right_detail(buffer);
  buffer.saved_j = buffer.cursor.j;
}

static void cursor_left_detail(buffer_view &buffer) {
  auto &cursor = buffer.cursor;
  auto &start_j = buffer.start_j;
  auto &i = cursor.i; auto &j = cursor.j;
  auto &buffer_i = buffer[i];

  const int max_line = start_j * buffer_width() / fw;
  if(j && j == max_line) {
    start_j--;
  }

  if(j > (int)0) {
    buffer_i.move_left();
    j--;
  }
}

static void cursor_left(buffer_view &buffer) {
  cursor_left_detail(buffer);
  buffer.saved_j = buffer.cursor.j;
}

static void cursor_to(buffer_view &buffer, int to_i, int to_j) {
  auto &cursor = buffer.cursor;
  auto &i = cursor.i; auto &j = cursor.j;
  if(to_i == i) {
    int diff = to_j - j;
    while(diff > 0) {
      cursor_right_detail(buffer);
      diff--;
    } 
    while(diff < 0) {
      cursor_left_detail(buffer);
      diff++;
    } 
    assert(to_j == j);
  }
}


static void cursor_down(buffer_view &buffer) {
  auto &start = buffer.start;
  auto &cursor = buffer.cursor;
  auto &i = cursor.i;
  const int max_size = buffer.size()-1;

  if(i == max_size) { // On the last line of file.  
    return;
  } 
  
  if((int)(i-start) == numrows()-1) { // On the last line of page.  
    start++;
  }

  buffer.move_right();
  i++;
  cursor_to(buffer, i, std::min(buffer.saved_j, buffer[i].size()-1));
  fix_gap(buffer);
}


static void cursor_up(buffer_view &buffer) {
  auto &start = buffer.start;
  auto &cursor = buffer.cursor;
  auto &i = cursor.i; //auto &j = cursor.j;

  if(i == 0) { // On the first line of file.  
    return;
  }

  if((int)(i-start) == 0) { // On the firtst line of page.
    start--;
  }


  buffer.move_left();
  i--;
  cursor_to(buffer, i, std::min(buffer.saved_j, buffer[i].size()-1));
  fix_gap(buffer);
}

static void return_key(buffer_view &buffer) {
  auto &start = buffer.start;
  auto &cursor = buffer.cursor;
  auto &i = cursor.i; auto &j = cursor.j;
  auto buf_i = buffer[i];

  gap_buffer<char> to_end;
  gap_buffer<char> from_start;

  for(auto k = 0; k < j; k++) {
    from_start.insert(buf_i[k]);
  }
  from_start.insert(' '); // add extra space.
  for(unsigned k = j; k < buf_i.size(); k++) {
    to_end.insert(buf_i[k]);
  }

  buffer.del();
  buffer.add(from_start);
  buffer.add(to_end);

  if((int)(i-start) == numrows()-1) {
    start++;
  }

  i++;
  buffer.move_left();
  cursor_to(buffer, i, 0);
  buffer.saved_j = j;
  fix_gap(buffer);
}

static void backspace_key(buffer_view &buffer) {
  auto &cursor = buffer.cursor;
  auto &start = buffer.start;
  auto &i = cursor.i; auto &j = cursor.j;

  if(j != 0) {
    j--;
    buffer[i].backspace();
  } else {
    if(!i) return;
    assert(j == 0);
    
    auto &previous_line = buffer[i-1];
    auto current_line = buffer[i];
    auto size = previous_line.size()-1;
    string s = "";
    for(unsigned k = 0; k < current_line.size(); k++) {
      if(k == 0) { // overwriting trailing space.
        assert(previous_line[size] == ' ');
        previous_line[size] = current_line[k];
      } else {     // inserting to the end.
        s.push_back(current_line[k]);
      }
    }
    previous_line.insert_many(s);

    if((int)(i-start) == 0) {
      start--;
    }

    i--;
    buffer.del();

    buffer.move_left();
    cursor_to(buffer, i, size);
  }
  buffer.saved_j = j;
  fix_gap(buffer);
}

static void delete_key(buffer_view &buffer) {
  auto &cursor = buffer.cursor;
  auto &start = buffer.start;
  auto &i = cursor.i; auto &j = cursor.j;

  if((unsigned)j != buffer[i].size()-1) {
    buffer[i].del();
  } else {
    if((unsigned)i == buffer.size()-1) return;


    auto next_line = buffer[i+1];
    auto &current_line = buffer[i];
    auto size = current_line.size()-1;
    string s = "";
    for(unsigned k = 0; k < next_line.size(); k++) {
      if(k == 0) {
        assert(current_line[size] == ' ');
        current_line[size] = next_line[k];
      } else {
        s.push_back(next_line[k]);
      }
    }
    current_line.insert_many(s);

    if((int)(i-start) == numrows()-1) {
      start++;
    }

    buffer.move_right();
    buffer.del();
    buffer.move_left();
    cursor_to(buffer, i, size);
  }
  buffer.saved_j = j;
  fix_gap(buffer);
}

static void put_tab(buffer_view &buffer) {
  auto &j = buffer.cursor.j;
  for(char i = 0; i < tabstop; i++) {
    buffer[buffer.cursor.i].add(' ');
    j++;
  }
}
/*
static void open_console() {
  Mode = Console;
}

static void close_console() {
  Mode = Editor;
}
*/


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
  buffer.decrease_start_by(start_change); // Why start? gap must correlate to cursor and not to start.
}

void handle_scroll_down(buffer_view &buffer) {
  auto &start = buffer.start;
  auto &i = buffer.cursor.i;

  i -= (i < scroll_speed)? i: scroll_speed;

  unsigned total = buffer.size()-1; int ts = total-start;
  int speed = (ts < scroll_speed)? ts: scroll_speed;
  auto start_change = (start == total)? 0: speed;

  buffer.increase_start_by(start_change); // The same as above;
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


void handle_resize(const SDL_Event &e, SDL_Window *win, ScrollBar &bar, buffer_view &buffer) {
  if(e.window.event == SDL_WINDOWEVENT_RESIZED) {
    SDL_GetWindowSize(win, &Width, &Height);
    reinit_bar(bar, buffer);

    auto &start = buffer.start;
    if((int)(buffer.cursor.i-start) == numrows()) {
      start++;
    }
  }
}


void handle_keydown(const SDL_Event &e, buffer_view &buffer, bool &done) {
  auto keysym = e.key.keysym;
  auto key = keysym.sym;

  auto &cursor = buffer.cursor;
  auto &i = cursor.i; auto &j = cursor.j;
  /*
  switch(Mode) {
    case Editor: {
      puts("Editor");
    } break;

    case Console: {
      puts("Console");
    } break;
  }
  */
  if(key == SDLK_LSHIFT || key == SDLK_RSHIFT) {
    SDL_SetModState((SDL_Keymod)KMOD_SHIFT);
    return;

  } else if(key == SDLK_ESCAPE) {
    done = true;
    return;

  } else if(key == SDLK_TAB) {
    put_tab(buffer);
    return;

  } else if(key == SDLK_BACKSPACE) {
    backspace_key(buffer);
    return;

  } else if(key == SDLK_DELETE) {
    delete_key(buffer);
    return;

  } else if(key == SDLK_RETURN) {
    return_key(buffer);
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


void handle_mousebuttondown(const SDL_Event &e, buffer_view &buffer, ScrollBar &bar, ScrollBar*& active) {
  auto &button = e.button;
  auto &b_type = button.button; 
  auto &b_click = button.clicks;
  int x = button.x; int y = button.y;

  if(clicked_small(bar, x, y)) {
    got_clicked(bar, x, y);
    active = &bar;

  } else if(clicked_bar(bar, x, y)) {

  } else if(in_buffer(x, y)) {
    if(b_type == SDL_BUTTON_LEFT && b_click == 3) {
      puts("Triple click!");

    } else if(b_type == SDL_BUTTON_LEFT && b_click == 2) {
      puts("Double click!");

    } else if(b_type == SDL_BUTTON_LEFT) {
      puts("Click");
      //get_pos(buffer, x, y);
    }
  }
}


void handle_mousebuttonup(const SDL_Event &e, ScrollBar*& active) {
  auto &button = e.button;
  if(button.button == SDL_BUTTON_LEFT) {
    active = nullptr;
  }
}
