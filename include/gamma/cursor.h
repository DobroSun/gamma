#ifndef GAMMA_CURSOR_H
#define GAMMA_CURSOR_H

template<class T>
struct vector2D_view;
using buffer_view = vector2D_view<std::string>;

struct SDL_Texture;
struct Cursor {
  int i, j;
};

bool operator==(const Cursor &c1, const Cursor &c2);
bool operator!=(const Cursor &c1, const Cursor &c2);

namespace cursor {
  std::string str(const char k);
  std::string str(const std::string &k);
  std::string str(const char *k);
  SDL_Texture *get_cursored(const buffer_view &buffer, const Cursor &c);
  bool out_buffer(const buffer_view &buffer, int i, int j);
  bool out_buffer(const buffer_view &buffer, const Cursor &c);
};

Cursor get_pos(double x, double y, int);
Cursor fix_cursor(const buffer_view &, const Cursor &);
SDL_Texture *init_cursor(const buffer_view &buffer, const Cursor &cursor);
SDL_Texture *render_cursor(SDL_Texture *t, const buffer_view &buffer, const Cursor &cursor);
#endif
