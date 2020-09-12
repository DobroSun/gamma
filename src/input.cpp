#include "gamma/pch.h"
#include "gamma/input.h"
#include "gamma/buffer.h"
#include "gamma/init.h"


static void handle_resize(SDL_Event e) {
  if(e.window.event == SDL_WINDOWEVENT_RESIZED) {
    int p_width = Width, p_height = Height;
    SDL_GetWindowSize(get_win(), &Width, &Height);
    
    auto &tab = get_current_tab();
    for(auto i = 0u; i < tab.buffers.size; i++) {
      tab.buffers[i].act_on_resize(p_width, p_height, Width, Height);
    }
  }
}

static void handle_mouse_wheel(SDL_Event e) {
  auto &buffer = get_current_buffer();
  if(e.wheel.y > 0) {
    do_times(dt_scroll, buffer.scroll_up);

  } else if(e.wheel.y < 0) {
    do_times(dt_scroll, buffer.scroll_down);

  } else {
  }
}

static void put_return();
static void put_backspace();
static void put_delete();
static void go_left();
static void go_right();
static void handle_editor_keydown(SDL_Event e) {
  auto key = e.key.keysym.sym;
  if(key == SDLK_ESCAPE) {
    should_quit = true;

  } else if(key == SDLK_RETURN) {
    put_return();
  
  } else if(key == SDLK_BACKSPACE) {
    put_backspace();
  
  } else if(key == SDLK_DELETE) {
    put_delete();
  
  } else if(key == SDLK_LEFT) {
    go_left();
  
  } else if(key == SDLK_RIGHT) {
    go_right();
  }
}

static void put_return() {
  auto &buffer = get_current_buffer();
  buffer.buffer.add('\n');
  buffer.cursor++;
}

static void put_backspace() {
  auto &buffer = get_current_buffer();
  buffer.buffer.backspace();
  if(buffer.cursor == 0) {
    // Do nothing.
  } else {
    buffer.cursor--;
  }
}

// @Incomplete:
static void put_delete() {
  auto &buffer = get_current_buffer();
  buffer.buffer.del();
}

static void go_left() {
  auto &buffer = get_current_buffer();
  buffer.buffer.move_left();
  if(buffer.cursor == 0) {
    // Do nothing.
  } else {
    buffer.cursor--;
  }
}

static void go_right() {
  auto &buffer = get_current_buffer();
  buffer.buffer.move_right();
  if(buffer.cursor == buffer.buffer.size()-1) {
    // @Incomplete:
  } else {
    buffer.cursor++;
  }
}

static void put_key(char);
static void handle_text_input(SDL_Event e) {
  char c = e.text.text[0];
  put_key(c);
}

static void put_key(char c) {
  auto &buffer = get_current_buffer();
  buffer.buffer.add(c);
  buffer.cursor++;
}

static void handle_console_keydown() {}
static void handle_keydown(SDL_Event e) {
  switch(get_editor_mode()) {
    case EditorMode::Editor: {
      handle_editor_keydown(e);
    } break;

    case EditorMode::Console: {
      handle_console_keydown();
    } break;
  }
}

void process_input() {
  SDL_Event e;

  while(SDL_PollEvent(&e)) {
    switch(e.type) {
      case SDL_QUIT: {
        should_quit = true;
      } break;

      case SDL_KEYDOWN: {
        handle_keydown(e);
      } break;

      case SDL_TEXTINPUT: {
        handle_text_input(e);
      } break;

      case SDL_WINDOWEVENT: {
        handle_resize(e);
      } break;

      case SDL_MOUSEWHEEL: {
        handle_mouse_wheel(e);
      } break;

      default: {
      } break;
    }
  }
}


