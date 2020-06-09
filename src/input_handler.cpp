#include "gamma/pch.h"
#include "gamma/input_handler.h"
#include "gamma/timer.h"
#include "gamma/globals.h"
#include "gamma/scroll_bar.h"
#include "gamma/cursor.h"
#include "gamma/utility.h"
#include "gamma/view.h"

void LoadFile(SDL_Renderer *renderer, TTF_Font *gfont, String &buffer, std::vector<SDL_Texture *> &textures, std::fstream &file) {
  std::string input; 
  unsigned count = 0;
  while(std::getline(file, input)) {
    buffer.push_back(input);

    // Prerender visible text.
    if(count < (unsigned)numrows()) {
      textures.push_back(load_courier(renderer, gfont, input, BlackColor));
      count++;
    }
  }

  textures.resize(buffer.size());
  for( ; count < buffer.size(); count++) {
    textures[count] = load_courier(renderer, gfont, buffer[count], BlackColor);
  }
}


static bool in_buffer(double x, double y) {
  return y >= TextUpperBound && x >= TextLeftBound && y < Height - TextBottomBound;
}


static void handle_scroll_up(Uint32 &start, Cursor &cursor) {
  int diff = numrows() - cursor.i - 1;
  cursor.i += (diff < scroll_speed)? diff: scroll_speed;
  start -= (start < scroll_speed)? start: scroll_speed;
}

static void handle_scroll_down(Uint32 &start, Cursor &cursor, const buffer_view &buffer) {
  cursor.i -= (cursor.i < scroll_speed)? cursor.i: scroll_speed;
  unsigned total = buffer.size()-1; int ts = total-start;
  int speed = (ts < scroll_speed)? ts: scroll_speed;
  start += (start == total)? 0: speed;
}


void handle_mousewheel(const SDL_Event &e, const buffer_view &buffer, ScrollBar &bar, Cursor &cursor, Uint32 &start) {
  auto &wheel = e.wheel;

  if(wheel.y > 0) {
    handle_scroll_up(start, cursor);
    start_to_bar(buffer, bar, start);
    if(start == 0) return;


  } else if(wheel.y < 0) {
    start_to_bar(buffer, bar, start);
    handle_scroll_down(start, cursor, buffer);
  }
}


void handle_resize(const SDL_Event &e, SDL_Window *win, ScrollBar &bar, const buffer_view &buffer) {
  if(e.window.event == SDL_WINDOWEVENT_RESIZED) {
    SDL_GetWindowSize(win, &Width, &Height);
    reinit_bar(bar, buffer);
  }
}


void handle_keydown(const SDL_Event &e, bool &done) {
  if(e.key.keysym.sym == SDLK_ESCAPE) {
    done = true;
  }
}


void handle_mousemotion(const SDL_Event &e, const buffer_view &buffer, ScrollBar*& active, Uint32 &s) {
  auto &motion = e.motion;
  auto &x = motion.x; auto &y = motion.y;

  if(active) {
    auto yy = y-TextUpperBound;
    if(yy <= TextUpperBound+active->h) {
      bar_to_start(buffer, active, yy, s);
    }
  }
}


void handle_mousebuttondown(const SDL_Event &e, Cursor &cursor, const buffer_view &b_view, ScrollBar &bar, int fw, ScrollBar*& active) {
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
      get_pos(x, y, fw, cursor);
      //fix_cursor(b_view, cursor);
    }
  }
}


void handle_mousebuttonup(const SDL_Event &e, ScrollBar*& active) {
  auto &button = e.button;
  if(button.button == SDL_BUTTON_LEFT) {
    active = nullptr;
  }
}

