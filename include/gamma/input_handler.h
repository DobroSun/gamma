#ifndef GAMMA_INPUT_HANDLER_H
#define GAMMA_INPUT_HANDLER_H
#include "gamma/globals.h"
#include "gamma/fwd_decl.h"

void LoadFile(String &, std::fstream &);

void handle_mousewheel(const SDL_Event &, const buffer_view &, ScrollBar &, Cursor &, Uint32 &);
void handle_resize(const SDL_Event &, SDL_Window *, ScrollBar &, const buffer_view &);
void handle_keydown(const SDL_Event &, buffer_view &buffer, Cursor &cursor, bool &);
void handle_mousemotion(const SDL_Event &, const buffer_view &, ScrollBar*&, Uint32 &);
void handle_mousebuttondown(const SDL_Event &, Cursor &, const buffer_view &, ScrollBar &, int, ScrollBar*&);
void handle_mousebuttonup(const SDL_Event &, ScrollBar*&);

#endif
