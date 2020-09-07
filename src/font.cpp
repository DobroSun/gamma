#include "gamma/pch.h"
#include "gamma/font.h"
#include "gamma/init.h"
#include "gamma/globals.h"

static TTF_Font *active_font = nullptr;
static texture_map alphabet;

static int tw, th;
static void copy_texture(SDL_Texture *t, int px, int py) {
  SDL_QueryTexture(t, nullptr, nullptr, &tw, &th);
  SDL_Rect dst {px, py, tw, th};
  SDL_RenderCopy(get_renderer(), t, nullptr, &dst);
}

void draw_text(TTF_Font *font, const char *text, SDL_Color c, int p1, int p2) {
  auto txt = render_text_solid(font, text, c);
  defer { SDL_DestroyTexture(txt); };

  copy_texture(txt, p1, p2);
}

void draw_text_shaded(TTF_Font *font, const char *text, SDL_Color c1, SDL_Color c2, int p1, int p2) {
  auto txt = render_text_shaded(font, text, c1, c2);
  defer { SDL_DestroyTexture(txt); };

  copy_texture(txt, p1, p2);
}


TTF_Font *load_font(const char *font_name, int ptsize) {
  char *font_path = concat(assets_fonts, font_name);
  defer { free(font_path); };
  assert(font_path);

  TTF_Font *font = TTF_OpenFont(font_path, ptsize);
  return font;
}


SDL_Texture *render_text_solid(TTF_Font *font, const char *text, SDL_Color color) {
  assert(font);

  SDL_Surface *surf = TTF_RenderText_Solid(font, text, color);
  defer { SDL_FreeSurface(surf); };
  assert(surf);

  SDL_Texture *txt = SDL_CreateTextureFromSurface(get_renderer(), surf);
  assert(txt);
  return txt;
}


SDL_Texture *render_text_shaded(TTF_Font *font, const char *s, SDL_Color c1, SDL_Color c2) {
  assert(font);

  SDL_Surface *surf = TTF_RenderText_Shaded(font, s, c1, c2);
  defer { SDL_FreeSurface(surf); };
  assert(surf);

  SDL_Texture *txt = SDL_CreateTextureFromSurface(get_renderer(), surf);
  assert(txt);
  return txt;
}

TTF_Font *&get_font() {
  return active_font;
}

texture_map &get_alphabet() {
  return alphabet;
}

void fill_alphabet(SDL_Color color) {
  assert(active_font);
  for_each(chars) {
    char c = *it;
    alphabet.insert(std::make_pair(c, render_text_solid(active_font, &c, color)));
  }
}

void clear_alphabet() {
  alphabet.clear();
}