#ifndef GAMMA_FORWARD_DECLARATION_H
#define GAMMA_FORWARD_DECLARATION_H

typedef Uint32 (*callback)(Uint32, void*);
struct SDL_Renderer;
using String = std::vector<std::string>;
typedef uint32_t Uint32;
struct SDL_Texture;
struct Cursor;
struct ScrollBar;

template<class T>
class vector2D_view;

using buffer_view = vector2D_view<std::string>;


#endif
