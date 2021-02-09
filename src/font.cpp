#include "pch.h"
#include "font.h"
#include "init.h"

static void copy_texture(SDL_Texture *t, int px, int py) {
  int tw, th;
  SDL_QueryTexture(t, NULL, NULL, &tw, &th);
  SDL_Rect dst {px, py, tw, th};
  SDL_RenderCopy(get_renderer(), t, NULL, &dst);
}

static SDL_Texture *render_text_solid(TTF_Font *font, const char *text, SDL_Color color) {
  assert(font);
  SDL_Surface *surf = TTF_RenderText_Solid(font, text, color); assert(surf);
  SDL_Texture *txt  = SDL_CreateTextureFromSurface(get_renderer(), surf); assert(txt);
  SDL_FreeSurface(surf);
  return txt;
}

static SDL_Texture *render_text_shaded(TTF_Font *font, const char *s, SDL_Color c1, SDL_Color c2) {
  assert(font);
  SDL_Surface *surf = TTF_RenderText_Shaded(font, s, c1, c2); assert(surf);
  SDL_Texture *txt  = SDL_CreateTextureFromSurface(get_renderer(), surf); assert(txt);
  SDL_FreeSurface(surf);
  return txt;
}


void draw_rect(int x, int y, int w, int h, SDL_Color c) {
  SDL_SetRenderDrawColor(get_renderer(), c.r, c.g, c.b, c.a);
  SDL_Rect r {x, y, w, h};
  SDL_RenderFillRect(get_renderer(), &r);
}

void draw_text(TTF_Font *font, const char *text, SDL_Color c, int p1, int p2) {
  auto txt = render_text_solid(font, text, c);
  copy_texture(txt, p1, p2);
  SDL_DestroyTexture(txt);
}

void draw_text_shaded(TTF_Font *font, const char *text, SDL_Color c1, SDL_Color c2, int p1, int p2) {
  auto txt = render_text_shaded(font, text, c1, c2);
  copy_texture(txt, p1, p2);
  SDL_DestroyTexture(txt);
}

void draw_text_shaded(TTF_Font *font, char c, SDL_Color c1, SDL_Color c2, int p1, int p2) {
  assert(font);

  int minx,maxx,miny,maxy,advance;
  int e = TTF_GlyphMetrics(font,c,&minx,&maxx,&miny,&maxy,&advance);
  assert(e != -1);

  auto surf = TTF_RenderGlyph_Shaded(font, c, c1, c2);            assert(surf);
  auto txt  = SDL_CreateTextureFromSurface(get_renderer(), surf); assert(txt);

  copy_texture(txt, p1, p2);

  SDL_FreeSurface(surf);
  SDL_DestroyTexture(txt);
}

static TTF_Font *active_font = NULL;
TTF_Font        *&get_font() { return active_font; }

void clear_font() { TTF_CloseFont(active_font); }
void make_font() {
  {
  char *font_path = concat(assets_fonts, font_name);
  defer { free(font_path); };
  active_font = TTF_OpenFont(font_path, font_size);
  }

  assert(active_font);
  TTF_SizeText(active_font, "G", &font_width, &font_height);
}
