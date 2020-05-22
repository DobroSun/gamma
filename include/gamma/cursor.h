#ifndef GAMMA_CURSOR_H
#define GAMMA_CURSOR_H

struct Cursor {
  int i, j;
};
using String = std::vector<std::string>;
struct SDL_Texture;

namespace cursor {
  std::string str(const char k);
  std::string str(const std::string &k);
  std::string str(const char *k);
  SDL_Texture *get_cursored(const String &buffer, const Cursor &c);
  bool out_buffer(const String &buffer, int i, int j);
  bool out_buffer(const String &buffer, const Cursor &c);
};

Cursor get_pos(const String &, double x, double y, int);
Cursor fix_cursor(const String &, const Cursor &);
SDL_Texture *init_cursor(const String &buffer, const Cursor &cursor);
SDL_Texture *render_cursor(SDL_Texture *t, const String &buffer, const Cursor &cursor);
#endif
