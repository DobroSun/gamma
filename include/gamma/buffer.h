#ifndef GAMMA_BUFFER_H
#define GAMMA_BUFFER_H
#include "gamma/fwd_decl.h"

enum EditorMode {
  Editor,
  Console,
};

bool load_buffer_from_file(const string &filename);
void handle_console_keydown(const SDL_Event &e);
void handle_editor_keydown(const SDL_Event &e, bool &done);

buffer_view &get_buffer();
EditorMode get_editor_mode();
#endif
