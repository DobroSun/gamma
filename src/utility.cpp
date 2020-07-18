#include "gamma/pch.h"
#include "gamma/utility.h"
#include "gamma/globals.h"
#include "gamma/view.h"
#include "gamma/gap_buffer.h"


char numrows() {
  return (Height - TextUpperBound - TextBottomBound) / (ptsize+blines);
}

int buffer_width() {
  return Width-TextLeftBound-25; // @Temporary: 25 is scroll_bar.width.
}


string read_args(int argc, char **argv) {
  return (argc < 2)? "": argv[1];
}

void create_alphabet(SDL_Renderer *renderer, TTF_Font *gfont, texture_map &alphabet, texture_map &selected) {
  for_each(chars) {
    auto c = *it;
    alphabet.insert(std::make_pair(c, load_courier(renderer, gfont, c, BlackColor)));
    selected.insert(std::make_pair(c, load_cursor(renderer, gfont, c, WhiteColor, BlackColor)));
  }
}


SDL_Texture *load_texture(SDL_Renderer *renderer, const string &text, const string &path, int ptsize, const SDL_Color &color) {
  TTF_Font *font = TTF_OpenFont((assets_fonts+path).data(), ptsize);
  assert(font);

  SDL_Surface *surf = TTF_RenderText_Solid(font, text.data(), color);
  assert(surf);
  TTF_CloseFont(font);

  SDL_Texture *txt = SDL_CreateTextureFromSurface(renderer, surf);
  assert(txt);

  SDL_FreeSurface(surf);
  return txt;
}

SDL_Texture *load_courier(SDL_Renderer *renderer, TTF_Font *font, const string &text, const SDL_Color &color) {
  SDL_Surface *surf = TTF_RenderText_Solid(font, text.data(), color);
  assert(surf);
  SDL_Texture *txt = SDL_CreateTextureFromSurface(renderer, surf);
  assert(txt);
  SDL_FreeSurface(surf);
  return txt;
}


SDL_Texture *load_cursor(SDL_Renderer *renderer, TTF_Font *gfont, const string &s, const SDL_Color &c1, const SDL_Color &c2) {
  SDL_Surface *surf = TTF_RenderText_Shaded(gfont, s.data(), c1, c2);
  assert(surf);
  SDL_Texture *txt = SDL_CreateTextureFromSurface(renderer, surf);
  assert(txt);
  SDL_FreeSurface(surf);
  return txt;
}

// Editor commands.
/*
bool save(const buffer_t &b, const std::string &filename) {
  std::fstream file{filename};
  if(!file) {
    return false;
  }
  for(unsigned i = 0; i < b.size(); i++) {
    auto &b_i = b[i];
    for(unsigned j = 0; j < b_i.size()-1; j++) {
      file << b_i[j];
    }
    file << "\n";
  }
  return true;
}
*/
/*
void delete_line(buffer_view &b, Cursor &c) {
}
*/
