#include "gamma/pch.h"
#include "gamma/cursor.h"
#include "gamma/globals.h"
#include "gamma/utility.h"
#include "gamma/view.h"


bool operator==(const Cursor &c1, const Cursor &c2) {
  return c1.i == c2.i && c1.j == c2.j;
}
bool operator!=(const Cursor &c1, const Cursor &c2) {
  return c1.i != c2.i || c1.j != c2.j;
}

namespace {

std::string str(const char k) {
  return {k};
}
std::string str(const char *k) {
  return {k};
}

SDL_Texture *get_cursored(SDL_Renderer *renderer, TTF_Font *gfont, const buffer_view &buffer, const Cursor &c) {
  auto &line = buffer[c.i]; auto &ll = line[c.j];
  auto s = (c.j >= line.size())? str(" ").c_str(): str(ll).c_str();

  SDL_Surface *t = TTF_RenderText_Shaded(gfont, s, WhiteColor, BlackColor);
  SDL_Texture *cc = SDL_CreateTextureFromSurface(renderer, t);
  SDL_FreeSurface(t);
  return cc;
}

bool out_buffer(const buffer_view &buffer, int i, int j) {
  return j > buffer[i].size();
}
bool out_buffer(const buffer_view &buffer, const Cursor &c) {
  return out_buffer(buffer, c.i, c.j);
}


}; // namespace


void get_pos(double x, double y, int fw, Cursor &c) {
  auto xx = x - TextLeftBound;
  auto yy = y - TextUpperBound;
  c.i = (yy + TextBottomBound - fsize/2) / fsize;
  c.j = xx / fw;
}

void fix_cursor(const buffer_view &buffer, Cursor &c) {
  if(out_buffer(buffer, c)) {
    auto &line = buffer[c.i];
    c.j = line.size();
  }
}

SDL_Texture *init_cursor(SDL_Renderer *renderer, TTF_Font *gfont, const buffer_view &buffer, const Cursor &cursor) {
  return get_cursored(renderer, gfont, buffer, cursor);
}

SDL_Texture *render_cursor(SDL_Renderer *renderer, TTF_Font *gfont, SDL_Texture *t, const buffer_view &buffer, const Cursor &cursor) {
  SDL_DestroyTexture(t);
  return get_cursored(renderer, gfont, buffer, cursor);
}
