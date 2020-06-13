#ifndef GAMMA_CURSOR_H
#define GAMMA_CURSOR_H
#include "gamma/fwd_decl.h"
struct Cursor {
  int i, j;
};

bool operator==(const Cursor &c1, const Cursor &c2);
bool operator!=(const Cursor &c1, const Cursor &c2);


void get_pos(double x, double y, int, Cursor &);
void fix_cursor(const buffer_view &, Cursor &);

SDL_Texture *init_cursor(SDL_Renderer *renderer, TTF_Font *gfont, const buffer_view &buffer, const Cursor &cursor);
SDL_Texture *render_cursor(SDL_Renderer *renderer, TTF_Font *gfont, SDL_Texture *t, const buffer_view &buffer, const Cursor &cursor);
#endif
