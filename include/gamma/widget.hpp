#ifndef GAMMA_WIDGET_HPP
#define GAMMA_WIDGET_HPP
#include <memory>
#include <vector>

struct SDL_Texture;
struct SDL_Rect;
class SdlImpl;
class Widget {
public:
  virtual ~Widget() {}
  virtual void update(SdlImpl &sdl_impl) = 0;
};
class Button: public Widget {
public:
  virtual ~Button() {}
  virtual bool focused(int posx, int posy) = 0;
  virtual void on_click() = 0;
};

class GreyBackGround: public Widget {
public:
  void update(SdlImpl &sdl_impl);
};

class OrangeBackGround: public Widget {
public:
  void update(SdlImpl &sdl_impl);
};

template<bool yes>
class ChoiceButton: public Button {
  const int width = 150;
  const int height = 50;
  const int x, y;

  SDL_Texture *texture;
  SDL_Rect *Srect;
  SDL_Rect *Drect;
public:
  ChoiceButton(SdlImpl &sdl_impl, int posx, int posy);
  void update(SdlImpl &sdl_impl);
  bool focused(int posx, int poxy);
  void on_click();
};


#endif
