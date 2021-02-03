#ifndef GAMMA_FONT_H
#define GAMMA_FONT_H

TTF_Font *load_font(const char *, int);
SDL_Texture *render_text_solid(TTF_Font *, const char *, SDL_Color);
SDL_Texture *render_text_shaded(TTF_Font *, const char *, SDL_Color, SDL_Color);


void copy_texture(SDL_Texture *, int, int);
void draw_rect(int, int, int, int, SDL_Color);
void draw_text(TTF_Font *, const char *, SDL_Color, int, int);
void draw_text_shaded(TTF_Font *, const char *, SDL_Color, SDL_Color, int, int);
void draw_text_shaded(TTF_Font *, char,         SDL_Color, SDL_Color, int, int);

TTF_Font *&get_font();

void make_alphabet(SDL_Color color);
void clear_font();
void make_font();
#endif
