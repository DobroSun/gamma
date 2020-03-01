#include "gamma/widget.hpp"
#include "gamma/globals.hpp"
#include "gamma/sdl_impl.hpp"

#include <cassert>
#include <iostream>

void GreyBackGround::update(SdlImpl &sdl_impl) {
  sdl_impl.set_draw_color(GREY);
}

void OrangeBackGround::update(SdlImpl &sdl_impl) {
  sdl_impl.set_draw_color(ORANGE);
}

template<>
ChoiceButton<true>::ChoiceButton(SdlImpl &sdl_impl, int posx, int posy):
    x(posx), y(posy),
    texture(nullptr),
    Srect(NULL),
    Drect(new SDL_Rect) {

  std::string name = "white-button.png";
  texture = sdl_impl.make_texture(name);

  Drect->x = posx;
  Drect->y = posy;
  Drect->w = width;
  Drect->h = height;
}

template<>
ChoiceButton<false>::ChoiceButton(SdlImpl &sdl_impl, int posx, int posy):
    x(posx), y(posy),
    texture(nullptr),
    Srect(NULL),
    Drect(new SDL_Rect) {

  std::string name = "white-button.png";
  texture = sdl_impl.make_texture(name);

  Drect->x = posx;
  Drect->y = posy;
  Drect->w = width;
  Drect->h = height;
}

template<bool yes>
void ChoiceButton<yes>::update(SdlImpl &sdl_impl) {
  sdl_impl.render_rect(texture, Drect);
}

template<bool yes>
bool ChoiceButton<yes>::focused(int posx, int posy) {
  return (x < posx && posx < width &&
          y < posy && posy < height)? 
          true: false;
}

template<>
void ChoiceButton<true>::on_click() {
}

template<>
void ChoiceButton<false>::on_click() {

}
