#include "gamma/pch.h"
#include "gamma/utility.h"
#include "gamma/globals.h"
#include "gamma/view.h"
#include "gamma/gap_buffer.h"
#include "gamma/cursor.h"


char numrows() {
  return (Height - TextUpperBound - TextBottomBound) / (ptsize+blines);
}

int buffer_width() {
  return Width-TextLeftBound-25; // @Temporary: 25 is scroll_bar.width.
}

std::string read_args(int argc, char **argv) {
  return (argc < 2)? "": argv[1];
}

void create_alphabet(SDL_Renderer *renderer, TTF_Font *gfont, std::unordered_map<char, SDL_Texture *> &alphabet, std::unordered_map<char, SDL_Texture *> &selected) {
  for_each(chars) {
    auto c = *it;
    alphabet.insert(std::make_pair(c, load_courier(renderer, gfont, std::string{c}, BlackColor)));
    selected.insert(std::make_pair(c, load_cursor(renderer, gfont, std::string{c}, WhiteColor, BlackColor)));
  }
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


SDL_Texture *load_cursor(SDL_Renderer *renderer, TTF_Font *gfont, const std::string &s, const SDL_Color &c1, const SDL_Color &c2) {
  SDL_Surface *t = TTF_RenderText_Shaded(gfont, s.c_str(), c1, c2);
  SDL_Texture *cc = SDL_CreateTextureFromSurface(renderer, t);
  SDL_FreeSurface(t);
  return cc;
}

// Editor commands.

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

/*
void delete_line(buffer_view &b, Cursor &c) {
}
*/
