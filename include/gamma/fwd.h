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
class dyn_array;

class gap_buffer;
class buffer_t;
struct console_t;

struct editor_t;
struct tab_buffer_t;

enum class EditorMode;

using texture_map = std::unordered_map<char, SDL_Texture*>;

struct Token;

#endif
