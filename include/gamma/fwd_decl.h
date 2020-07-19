#ifndef GAMMA_FORWARD_DECLARATION_H
#define GAMMA_FORWARD_DECLARATION_H

typedef uint32_t Uint32;
typedef Uint32 (*callback)(Uint32, void*);
struct SDL_Renderer;
struct SDL_Texture;

struct _TTF_Font;
typedef struct _TTF_Font TTF_Font;

struct SDL_Color;
struct Cursor {
  int i, j;
};
struct buffer_view;

template<class T>
class gap_buffer;

template<class T>
class array;

class string;

using buffer_t = gap_buffer<gap_buffer<char>>;
using texture_map = std::unordered_map<char, SDL_Texture*>;
#endif
