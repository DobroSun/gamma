#include "gamma/pch.h"
#include "gamma/utility.h"
#include "gamma/globals.h"
extern SDL_Renderer *renderer;
extern TTF_Font *gfont;

char numrows() {
  return (Height - TextUpperBound - TextBottomBound) / (ptsize+blines);
}



SDL_Texture *load_texture(const std::string &text, const std::string &path, int ptsize, const SDL_Color &color) {
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

SDL_Texture *load_courier(const std::string &text, const SDL_Color &color) {
  SDL_Surface *surf = TTF_RenderText_Solid(gfont, text.c_str(), color);

  // For CuriorFont this 2 are failing, have no clue why.
/*
  if(!surf) {
    std::cerr << "Error rendering given text!" << std::endl;
  }
  if(!txt) {
    std::cerr << "Error creating texture from rendered surface!" << std::endl;
  }
*/

  SDL_Texture *txt = SDL_CreateTextureFromSurface(renderer, surf);
  SDL_FreeSurface(surf);
  return txt;
}
