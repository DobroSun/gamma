#include "gamma/pch.h"
#include "gamma/load_surf.h"
#include "gamma/globals.h"

extern SDL_Renderer *renderer;
/*
SDL_Texture *load_bmp(const std::string &path) {
  SDL_Surface *bmps = SDL_LoadBMP((assets_images+path).c_str());
  SDL_Texture *txt = SDL_CreateTextureFromSurface(renderer, bmps);
  SDL_FreeSurface(bmps);
  return txt;
}
*/

SDL_Texture *load_text(const std::string &text, const std::string &path, int ptsize, const SDL_Color &color) {
  TTF_Font *font = TTF_OpenFont((assets_fonts+path).c_str(), ptsize);
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
