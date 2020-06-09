#include "gamma/pch.h"
#include "gamma/utility.h"
#include "gamma/globals.h"


char numrows() {
  return (Height - TextUpperBound - TextBottomBound) / (ptsize+blines);
}

std::string read_args(int argc, char **argv) {
  return (argc < 2)? "": argv[1];
}


SDL_Texture *load_texture(SDL_Renderer *renderer, const std::string &text, const std::string &path, int ptsize, const SDL_Color &color) {
  TTF_Font *font = TTF_OpenFont((assets_fonts+path).c_str(), ptsize);
  if(!font) {
    std::cerr << "Error creating font from given path and size!" << std::endl;
  }
  SDL_Surface *surf = TTF_RenderText_Solid(font, text.c_str(), color);
  TTF_CloseFont(font);

  if(!surf) {
    std::cerr << "Error rendering given text!" << std::endl;
  }


  SDL_Texture *txt = SDL_CreateTextureFromSurface(renderer, surf);
  SDL_FreeSurface(surf);
  if(!txt) {
    std::cerr << "Error creating texture from rendered surface!" << std::endl;
  }
  return txt;
}

SDL_Texture *load_courier(SDL_Renderer *renderer, TTF_Font *font, const std::string &text, const SDL_Color &color) {
  SDL_Surface *surf = TTF_RenderText_Solid(font, text.c_str(), color);

  SDL_Texture *txt = SDL_CreateTextureFromSurface(renderer, surf);
  SDL_FreeSurface(surf);
  return txt;
}
