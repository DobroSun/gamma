#ifndef GAMMA_FORWARD_DECLARATION_H
#define GAMMA_FORWARD_DECLARATION_H

typedef Uint32 (*callback)(Uint32, void*);
struct SDL_Renderer;
typedef uint32_t Uint32;
struct SDL_Texture;
struct Cursor {
  int i, j;
};
struct buffer_view;

template<class T>
class gap_buffer;

using buffer_t = gap_buffer<gap_buffer<char>>;
#endif
