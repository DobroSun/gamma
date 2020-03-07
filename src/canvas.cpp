#include "gamma/pch.hpp"

#include "gamma/canvas.hpp"
#include "gamma/widget.hpp"

void MainCanvas::init(SdlImpl &sdl_impl) {
  back_ground = std::move(std::unique_ptr<Widget>(new GreyBackGround));
}


void MainCanvas::update(SdlImpl &sdl_impl) {
  back_ground->update(sdl_impl);
}


ExitCanvas::~ExitCanvas() {
  for(auto el: choice) {
    assert(el && "Cannot delete non existing object!");
    delete el;
  }
}

void ExitCanvas::init(SdlImpl &sdl_impl) {
  back_ground = std::move(std::unique_ptr<Widget>(new OrangeBackGround));
  choice.push_back(new ChoiceButton<false>(sdl_impl, 10, 130));
  choice.push_back(new ChoiceButton<true>(sdl_impl, 240, 130));
}


void ExitCanvas::update(SdlImpl &sdl_impl) {
  back_ground->update(sdl_impl);
  for(auto btn: choice) {
    btn->update(sdl_impl);
  }
}
