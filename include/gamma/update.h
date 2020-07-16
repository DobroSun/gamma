#ifndef GAMMA_UPDATE_H
#define GAMMA_UPDATE_H
#include "gamma/fwd_decl.h"

void update(SDL_Renderer *, const buffer_view &, std::unordered_map<char, SDL_Texture *> &, std::unordered_map<char, SDL_Texture *> &);

#endif
