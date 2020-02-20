#ifndef GAMMA_RENDERER_IMPL_HPP
#define GAMMA_RENDERER_IMPL_HPP

struct SDL_Renderer;
struct SDL_Texture;
struct SDL_Window;
struct SDL_Rect;
class IRendererImpl {
public:
  virtual ~IRendererImpl() {}
  virtual void set_color(int a, int b, int c, int d) = 0;
  virtual void copy(SDL_Texture *texture, SDL_Rect *SrcR, SDL_Rect *DestR) = 0;
  virtual void present() = 0;
  virtual void clear() = 0;
};


class RendererImpl: public IRendererImpl {
  SDL_Renderer *sdl_renderer;
public:
  RendererImpl(SDL_Window *win);
  ~RendererImpl();

  void set_color(int a, int b, int c, int d);
  void copy(SDL_Texture *texture, SDL_Rect *SrcR, SDL_Rect *DestR);
  void present();
  void clear();
};
#endif
