#ifndef GAMMA_UTILITY_H
#define GAMMA_UTILITY_H
namespace std {
  typedef basic_string<char> string;
};
struct SDL_Texture;
struct SDL_Color;
// Forward decls.


char numrows();
char maxnumrows();

SDL_Texture *load_bmp(const std::string &path);
SDL_Texture *load_text(const std::string &text, const std::string &path, int ptsize, const SDL_Color &color);
SDL_Texture *load_normal(const std::string &text);
#endif
