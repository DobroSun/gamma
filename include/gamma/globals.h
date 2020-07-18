#ifndef GAMMA_GLOBALS_H
#define GAMMA_GLOBALS_H

const int MaxHeight = 1025;

inline int Width = 1200;
inline int Height = 800;
inline char TextUpperBound = 100;
inline char TextLeftBound = 50;
inline char TextBottomBound = 50;

inline char ptsize = 25;
inline char blines = 5;
inline char fsize = ptsize+blines;
inline char tabstop = 2;
inline int  fw, fh;

inline unsigned char scroll_speed = 3;

const string assets_images = "../assets/images/";
const string assets_fonts = "../assets/fonts/";
const string quicksand = "QuickSand/Quicksand-Regular.ttf";
const string courier = "Courier/Courier-Regular.ttf";


const SDL_Color WhiteColor {255, 255, 255, 0};
const SDL_Color BlackColor {0, 0, 0, 0};


inline const char *chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890!@#$%^&?*()\\/ -+_=~|\t`\"'.,:;{}[]<>\0";

const char untouchable = 28; // @Depends on size of key_lookup table.
const char key_offset = 21;
static const SDL_Keycode key_lookup[69] = {
  SDLK_SPACE,
  SDLK_a,
  SDLK_b,
  SDLK_c,
  SDLK_d,
  SDLK_e,
  SDLK_f,
  SDLK_g,
  SDLK_h,
  SDLK_i,
  SDLK_j,
  SDLK_k,
  SDLK_l,
  SDLK_m,
  SDLK_n,
  SDLK_o,
  SDLK_p,
  SDLK_q,
  SDLK_r,
  SDLK_s,
  SDLK_t,
  SDLK_u,
  SDLK_v,
  SDLK_w,
  SDLK_x,
  SDLK_y,
  SDLK_z,
  SDLK_BACKSLASH,
  // 0 is 28th.
  SDLK_0,
  SDLK_1,
  SDLK_2,
  SDLK_3,
  SDLK_4,
  SDLK_5,
  SDLK_6,
  SDLK_7,
  SDLK_8,
  SDLK_9,
  SDLK_SEMICOLON,
  SDLK_EQUALS,
  SDLK_SLASH,
  SDLK_QUOTE,
  SDLK_COMMA,
  SDLK_PERIOD,
  SDLK_MINUS,
  SDLK_LEFTBRACKET,
  SDLK_RIGHTBRACKET,
  SDLK_BACKQUOTE,
  '\0',
  // Below \0 SDLK's cannot be reached from keyboard.
  // So access to them [(SDLK's from 28th to '\0') + 21]
  SDLK_RIGHTPAREN,
  SDLK_EXCLAIM,
  SDLK_AT,
  SDLK_HASH,
  SDLK_DOLLAR,
  SDLK_PERCENT,
  SDLK_CARET,
  SDLK_AMPERSAND,
  SDLK_ASTERISK,
  SDLK_LEFTPAREN,
  SDLK_COLON,
  SDLK_PLUS,
  SDLK_QUESTION,
  SDLK_QUOTEDBL,
  SDLK_LESS,
  SDLK_GREATER,
  SDLK_UNDERSCORE,
  (SDL_Keycode)'{',
  (SDL_Keycode)'}',
  (SDL_Keycode)'~',
};

#endif
