#ifndef GAMMA_UTILITY_H
#define GAMMA_UTILITY_H
namespace std {
  typedef basic_string<char> string;
};
struct SDL_Texture;
struct SDL_Renderer;
struct SDL_Color;
// Forward decls.


char numrows();

SDL_Texture *load_texture(const std::string &, const std::string &, int , const SDL_Color &);
SDL_Texture *load_courier(SDL_Renderer *, const std::string &, const SDL_Color &);
#endif
