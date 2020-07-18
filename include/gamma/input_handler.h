#ifndef GAMMA_INPUT_HANDLER_H
#define GAMMA_INPUT_HANDLER_H
#include "gamma/globals.h"
#include "gamma/fwd_decl.h"


void handle_resize(const SDL_Event &, SDL_Window *);
void handle_keydown(const SDL_Event &, bool &);

enum EditorMode {
  Editor,
  Console,
};

EditorMode get_editor_mode();

#endif

