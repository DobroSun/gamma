#include "pch.h"
#include "font.h"
#include "init.h"

static TTF_Font *active_font = nullptr;
static texture_map alphabet;

void copy_texture(SDL_Texture *t, int px, int py) {
  int tw, th;
  SDL_QueryTexture(t, nullptr, nullptr, &tw, &th);
  SDL_Rect dst {px, py, tw, th};
  SDL_RenderCopy(get_renderer(), t, nullptr, &dst);
}

void draw_rect(int x, int y, int w, int h, SDL_Color c) {
  SDL_SetRenderDrawColor(get_renderer(), c.r, c.g, c.b, c.a);
  SDL_Rect r {x, y, w, h};
  SDL_RenderFillRect(get_renderer(), &r);
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

void make_alphabet(SDL_Color color) {
  assert(active_font);
  for_each(chars) {
    char c[2];
    c[0] = *it;
    c[1] = '\0';
    alphabet.insert(std::make_pair(c[0], render_text_solid(active_font, reinterpret_cast<const char *>(&c), color)));
  }
}

void clear_alphabet() {
  alphabet.clear();
}

void clear_font() {
  assert(active_font);
  active_font = nullptr;
  font_width = 0, font_height = 0;
}

void make_font() {
  active_font = load_font("Courier-Regular.ttf", 25);
  assert(active_font);

  TTF_SizeText(active_font, "G", &font_width, &font_height);

  make_alphabet(BlackColor);
}
