#ifndef GAMMA_FWD_DECL_H
#define GAMMA_FWD_DECL_H

struct SDL_Window;
struct SDL_Renderer;
struct SDL_Texture;
struct SDL_Color;
struct _TTF_Font;
typedef struct _TTF_Font TTF_Font;
typedef Sint32 SDL_Keycode;

template<class T>
struct array;

struct string;

struct gap_buffer;
struct buffer_t;
struct console_t;

struct tab_t;
struct file_buffer_t;


typedef std::unordered_map<char, SDL_Texture*> texture_map;

struct Token;

#endif
