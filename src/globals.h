#ifndef GAMMA_GLOBALS_H
#define GAMMA_GLOBALS_H

inline int Width = 1200;
inline int Height = 800;

inline int font_width;
inline int font_height;

inline char tabstop = 2;
inline char dt_scroll = 3;

inline bool show_fps    = false;
inline bool should_quit = false;

inline const char *assets_fonts = "/gamma/assets/fonts/";
inline const char *font_name = "Courier-Regular.ttf";

inline       u16   font_size = 25;
inline SDL_Color   background_color   = {255,255,255,255};
inline SDL_Color   text_color         = {0,0,0,255};
inline SDL_Color   cursor_text_color  = background_color;
inline SDL_Color   cursor_color       = text_color;
inline SDL_Color   console_text_color = text_color;
inline SDL_Color   console_color      = background_color;
inline SDL_Color   searched_text_color = background_color;
inline SDL_Color   searched_color     = {250, 199, 57, 1};

inline const char *settings_filename = "syntax.m";

const SDL_Color WhiteColor = {255, 255, 255, 255};
const SDL_Color BlackColor = {0, 0, 0, 255};

inline void* allocate(size_t bytes) {
  void *r = malloc(bytes);
  memset(r, 0, bytes);
  return r;
}

inline void  deallocate(void *ptr) {
  return free(ptr);
}


#endif
