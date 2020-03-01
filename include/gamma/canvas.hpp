#ifndef GAMMA_CANVAS_HPP
#define GAMMA_CANVAS_HPP
#include <memory>
#include <vector>

#include "gamma/widget.hpp"

class SdlImpl;
class Widget;
class Canvas {
public:
  virtual ~Canvas() {}
  virtual void update(SdlImpl &sdl_impl) = 0;
  virtual void init(SdlImpl &sdl_impl) = 0;
};

class MainCanvas: public Canvas {
  std::unique_ptr<Widget> back_ground;
public:
  void update(SdlImpl &sdl_impl);
  void init(SdlImpl &sdl_impl);
};

class ExitCanvas: public Canvas {
  std::unique_ptr<Widget> back_ground;
  std::vector<Widget*> choice;
public:
  ~ExitCanvas();
  void update(SdlImpl &sdl_impl);
  void init(SdlImpl &sdl_impl);
};
#endif
