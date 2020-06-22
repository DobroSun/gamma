#ifndef GAMMA_UTILITY_H
#define GAMMA_UTILITY_H
#include "gamma/fwd_decl.h"

#define for_each(c_array) for(auto it = c_array; *it != '\0'; it++)
  

char numrows();
std::string read_args(int, char **);
void create_alphabet(SDL_Renderer *, TTF_Font *, std::unordered_map<char, SDL_Texture *> &, std::unordered_map<char, SDL_Texture *> &);



SDL_Texture *load_cursor(SDL_Renderer *, TTF_Font *, const std::string &, const SDL_Color &, const SDL_Color &);
SDL_Texture *load_texture(SDL_Renderer*, const std::string &, const std::string &, int , const SDL_Color &);
SDL_Texture *load_courier(SDL_Renderer *, TTF_Font *, const std::string &, const SDL_Color &);

// Editor commands.
bool save(const buffer_t &b, const std::string &);
void go_to_line(buffer_view &, Cursor &, int);


template<class T, class C>
bool in(const T *x, C val) {
  for_each(x) {
    if(*it == val) {
      return true;
    }
  }
  return false;
}

template<class T>
const T *slice(const T *x, unsigned index) {
  auto tmp = x;
  tmp += index;
  return tmp;
}
#endif
