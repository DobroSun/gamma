#ifndef GAMMA_UTILITY_H
#define GAMMA_UTILITY_H
#include "fwd_decl.h"

char numrows();
int buffer_width();
char *read_args(int, char **);
void create_alphabet(SDL_Renderer *, TTF_Font *, texture_map &, texture_map &);



SDL_Texture *load_cursor(SDL_Renderer *, TTF_Font *, const string &, const SDL_Color &, const SDL_Color &);
SDL_Texture *load_texture(SDL_Renderer*, const string &, const string &, int , const SDL_Color &);
SDL_Texture *load_courier(SDL_Renderer *, TTF_Font *, const string &, const SDL_Color &);

// Editor commands.
//bool save(const buffer_t &b, const string &);


#define for_each(c_array) for(auto it = c_array; *it != '\0'; it++)
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

#define printi(A) do { printf("%i\n", (A)); } while(0)
#define printu(A) do { printf("%u\n", (A)); } while(0)
#define printl(A) do { printf("%lu\n", (A)); } while(0)
#define printd(A) do { printf("%f\n", (A)); } while(0)
#define printc(A) do { printf("%c\n", (A)); } while(0)
#define printb(A) do { if((A)) puts("true"); else puts("false"); } while(0)
#define begin_clock auto begin = std::chrono::steady_clock::now()
#define printFPS                               \
  auto end = std::chrono::steady_clock::now(); \
  std::cout << 1000000 / std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << std::endl

#endif
