#ifndef GAMMA_UTILITY_H
#define GAMMA_UTILITY_H
#include "gamma/fwd_decl.h"

char numrows();
std::string read_args(int argc, char **argv);

SDL_Texture *load_texture(SDL_Renderer*, const std::string &, const std::string &, int , const SDL_Color &);
SDL_Texture *load_courier(SDL_Renderer *, TTF_Font *, const std::string &, const SDL_Color &);
#endif
