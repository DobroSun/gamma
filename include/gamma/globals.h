#ifndef GAMMA_GLOBALS_H
#define GAMMA_GLOBALS_H

const int MaxHeight = 1025;

inline int Width = 1200;
inline int Height = 800;
inline char TextUpperBound = 100;
inline char TextLeftBound = 50;
inline char TextBottomBound = 20;

inline char ptsize = 25;
inline char blines = 5;
inline char fsize = ptsize+blines;

inline unsigned char scroll_speed = 3;

const std::string assets_images = "../assets/images/";
const std::string assets_fonts = "../assets/fonts/";
const std::string quicksand = "QuickSand/Quicksand-Regular.ttf";
const std::string courier = "Courier/Courier-Regular.ttf";


const SDL_Color WhiteColor {255, 255, 255};
const SDL_Color BlackColor {0, 0, 0};


using String = std::vector<std::string>;

#endif
