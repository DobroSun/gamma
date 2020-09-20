#ifndef GAMMA_GLOBALS_H
#define GAMMA_GLOBALS_H

const int MaxHeight = 1025;
inline bool should_quit = false;

inline int Width = 1200;
inline int Height = 800;

inline int font_width;
inline int font_height;
inline char pixels_between_lines = 5;

inline char TextUpperBound = 100;
inline char TextLeftBound = 50;
inline char TextBottomBound = 50;

inline char tabstop = 2;
inline char dt_scroll = 3;


// @CleanUp.
inline const char *assets_fonts = "../assets/fonts/";


const SDL_Color WhiteColor {255, 255, 255, 0};
const SDL_Color BlackColor {0, 0, 0, 0};

inline const char *chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890!@#$%^&?*()\\/ -+_=~|\t`\"'.,:;{}[]<>";
#endif
