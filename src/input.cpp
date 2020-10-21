#include "gamma/pch.h"
#include "gamma/input.h"
#include "gamma/buffer.h"
#include "gamma/init.h"
#include "gamma/console.h"
#include "gamma/commands.h"

static EditorMode mode = EditorMode::Editor;

EditorMode *get_editor_mode() {
  return &mode;
}

static void handle_resize(SDL_Event e) {
  if(e.window.event == SDL_WINDOWEVENT_RESIZED) {
    SDL_GetWindowSize(get_win(), &Width, &Height);

    auto tab = get_current_tab();
    tab->on_resize(Width, Height);
  }
}

static void handle_mouse_wheel(SDL_Event e) {
  auto buffer = get_current_buffer();
  if(e.wheel.y > 0) {
    do_times(dt_scroll, buffer->scroll_up);

  } else if(e.wheel.y < 0) {
    do_times(dt_scroll, buffer->scroll_down);

  } else {
  }
}


static void handle_editor_keydown(SDL_Event e) {
  auto key = e.key.keysym.sym;
  auto mod = e.key.keysym.mod;

  if(mod & KMOD_CTRL && mod & KMOD_SHIFT) {

  } else if(mod & KMOD_SHIFT) {

  } else if(mod & KMOD_CTRL) {
    if(key == SDLK_r) {
      console_open();

    } else if(key == SDLK_c) {
      save();
    }

  } else {
    if(key == SDLK_ESCAPE) {
      should_quit = true;

    } else if(key == SDLK_RETURN) {
      get_current_buffer()->put_return();
    
    } else if(key == SDLK_BACKSPACE) {
      get_current_buffer()->put_backspace();
    
    } else if(key == SDLK_DELETE) {
      get_current_buffer()->put_delete();
    
    } else if(key == SDLK_LEFT) {
      get_current_buffer()->go_left();
    
    } else if(key == SDLK_RIGHT) {
      get_current_buffer()->go_right();
    
    } else if(key == SDLK_DOWN) {
      get_current_buffer()->go_down();
    
    } else if(key == SDLK_UP) {
      get_current_buffer()->go_up();
    }
  }
}

static void handle_editor_input(SDL_Event e) {
  char c = e.text.text[0];
  get_current_buffer()->put(c);
}

static void handle_console_keydown(SDL_Event e) {
  auto key = e.key.keysym.sym;
  if(key == SDLK_ESCAPE) {
    console_close();

  } else if(key == SDLK_RETURN) {
    console_run_command();
  
  } else if(key == SDLK_BACKSPACE) {
    console_backspace();
  
  } else if(key == SDLK_DELETE) {
    console_del();
  
  } else if(key == SDLK_LEFT) {
    console_go_left();
  
  } else if(key == SDLK_RIGHT) {
    console_go_right();
  }
}


static void handle_console_input(SDL_Event e) {
  char c = e.text.text[0];
  console_put(c);
}


static void handle_keydown(SDL_Event e) {
  switch(mode) {
    case EditorMode::Editor: {
      handle_editor_keydown(e);
    } break;

    case EditorMode::Console: {
      handle_console_keydown(e);
    } break;
  }
}

static void handle_text_input(SDL_Event e) {
  switch(mode) {
    case EditorMode::Editor: {
      handle_editor_input(e);
    } break;

    case EditorMode::Console: {
      handle_console_input(e);
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
