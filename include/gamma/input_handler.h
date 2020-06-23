#ifndef GAMMA_INPUT_HANDLER_H
#define GAMMA_INPUT_HANDLER_H
#include "gamma/globals.h"
#include "gamma/fwd_decl.h"

bool LoadFile(buffer_t &, const std::string &);

void handle_scroll_up(buffer_view &);
void handle_scroll_down(buffer_view &);

void handle_mousewheel(const SDL_Event &, buffer_view &, ScrollBar &);
void handle_resize(const SDL_Event &, SDL_Window *, ScrollBar &, const buffer_view &);
void handle_keydown(const SDL_Event &, buffer_view &, bool &, int);
void handle_mousemotion(const SDL_Event &, buffer_view &, ScrollBar*&);
void handle_mousebuttondown(const SDL_Event &, ScrollBar &, int, ScrollBar*&);
void handle_mousebuttonup(const SDL_Event &, ScrollBar*&);

#endif
