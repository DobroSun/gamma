#ifndef GAMMA_FORWARD_DECLARATION_H
#define GAMMA_FORWARD_DECLARATION_H

typedef Uint32 (*callback)(Uint32, void*);
struct SDL_Renderer;
typedef uint32_t Uint32;
struct SDL_Texture;
struct Cursor;
struct ScrollBar;

template<class T>
class vector2D_view;

template<class T>
class gap_buffer;

class buffer_view;

using buffer_t = gap_buffer<gap_buffer<char>>;
#endif
