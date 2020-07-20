#include "gamma/pch.h"
#include "gamma/input_handler.h"
#include "gamma/globals.h"
#include "gamma/view.h"
#include "gamma/buffer.h"


void handle_resize(const SDL_Event &e, SDL_Window *win) {
  auto &buffer = get_buffer();
  if(e.window.event == SDL_WINDOWEVENT_RESIZED) {
    SDL_GetWindowSize(win, &Width, &Height);

    auto &start = buffer.start;
    if((int)(buffer.cursor.i-start) == numrows()) {
      start++;
    }
  }
}


void handle_keydown(const SDL_Event &e, bool &done) {
  switch(get_editor_mode()) {
    case Editor: {
      handle_editor_keydown(e, done);
    } break;

    case Console: {
      handle_console_keydown(e);
    } break;
  }
}
