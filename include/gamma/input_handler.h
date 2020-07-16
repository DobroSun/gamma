#ifndef GAMMA_INPUT_HANDLER_H
#define GAMMA_INPUT_HANDLER_H
#include "gamma/globals.h"
#include "gamma/fwd_decl.h"


bool LoadFile(buffer_t &, const std::string &);

void handle_resize(const SDL_Event &, SDL_Window *, buffer_view &);
void handle_keydown(const SDL_Event &, buffer_view &, bool &);

enum EditorMode {
  Editor,
  Console,
};

EditorMode get_editor_mode();

#endif

