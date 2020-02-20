#ifndef GAMMA_IWINDOW_HPP
#define GAMMA_IWINDOW_HPP

class Window {
public:
  virtual ~Window() {}
  virtual bool run() = 0;
};
#endif
