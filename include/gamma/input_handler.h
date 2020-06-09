#ifndef GAMMA_INPUT_HANDLER_H
#define GAMMA_INPUT_HANDLER_H
#include "gamma/globals.h"
#include "gamma/fwd_decl.h"

void LoadFile(SDL_Renderer *, TTF_Font *, String &, std::vector<SDL_Texture *> &, std::fstream &);

void handle_mousewheel(const SDL_Event &, const buffer_view &, ScrollBar &, Cursor &, Uint32 &);
void handle_resize(const SDL_Event &, SDL_Window *, ScrollBar &);
void handle_keydown(const SDL_Event &, bool &);
void handle_mousemotion(const SDL_Event &, const buffer_view &, ScrollBar*&, Uint32 &);
void handle_mousebuttondown(const SDL_Event &, Cursor &, const buffer_view &, ScrollBar &, int, ScrollBar*&);
void handle_mousebuttonup(const SDL_Event &, ScrollBar*&);

void slice_buffer(SDL_Renderer *, TTF_Font *, buffer_view &, const Cursor &, SDL_Texture *, Uint32);
#endif
