#ifndef GAMMA_UTILITY_H
#define GAMMA_UTILITY_H
#include "gamma/fwd_decl.h"


char numrows();
int buffer_width();
std::string read_args(int, char **);
void create_alphabet(SDL_Renderer *, TTF_Font *, std::unordered_map<char, SDL_Texture *> &, std::unordered_map<char, SDL_Texture *> &);



SDL_Texture *load_cursor(SDL_Renderer *, TTF_Font *, const std::string &, const SDL_Color &, const SDL_Color &);
SDL_Texture *load_texture(SDL_Renderer*, const std::string &, const std::string &, int , const SDL_Color &);
SDL_Texture *load_courier(SDL_Renderer *, TTF_Font *, const std::string &, const SDL_Color &);

// Editor commands.
bool save(const buffer_t &b, const std::string &);


#define for_each(c_array) for(auto it = c_array; *it != '\0'; it++)
#define printi(A) do { printf("%i\n", (A)); } while(0)
#define printu(A) do { printf("%u\n", (A)); } while(0)
#define printd(A) do { printf("%f\n", (A)); } while(0)
#define printc(A) do { printf("%c\n", (A)); } while(0)
#define printb(A) do { if((A)) puts("true"); else puts("false"); } while(0)


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
