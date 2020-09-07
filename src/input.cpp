#include "gamma/pch.h"
#include "gamma/input.h"
#include "gamma/buffer.h"
#include "gamma/init.h"


static void handle_resize(const SDL_Event &e) {
  if(e.window.event == SDL_WINDOWEVENT_RESIZED) {
    int p_width = Width, p_height = Height;
    SDL_GetWindowSize(get_win(), &Width, &Height);
    
    auto &tab = get_current_tab();
    for(auto i = 0u; i < tab.buffers.size; i++) {
      tab.buffers[i].act_on_resize(p_width, p_height, Width, Height);
    }
  }
}

static void handle_editor_keydown(const SDL_Event &e) {
  if(e.key.keysym.sym == SDLK_ESCAPE) {
    should_quit = true;
  }
}

static void handle_console_keydown(const SDL_Event &e) {}

static void handle_keydown(const SDL_Event &e) {
  switch(get_editor_mode()) {
    case EditorMode::Editor: {
      handle_editor_keydown(e);
    } break;

    case EditorMode::Console: {
      handle_console_keydown(e);
    } break;
  }
}

void process_input() {
  SDL_Event e;

  if(SDL_PollEvent(&e)) {
    switch(e.type) {
      case SDL_QUIT: {
        should_quit = true;
      } break;

      case SDL_KEYDOWN: {
        handle_keydown(e);
      } break;

      case SDL_WINDOWEVENT: {
        handle_resize(e);
      } break;
    }
  }
}


