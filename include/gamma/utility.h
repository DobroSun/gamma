#ifndef GAMMA_UTILITY_H
#define GAMMA_UTILITY_H
namespace std {
  typedef basic_string<char> string;
};
struct SDL_Texture;
struct SDL_Color;
// Forward decls.


char numrows();

SDL_Texture *load_texture(const std::string &text, const std::string &path, int ptsize, const SDL_Color &color);
SDL_Texture *load_courier(const std::string &text, const SDL_Color &color);
#endif
