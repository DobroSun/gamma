#ifndef GAMMA_GAMMA_OBJECT_HPP
#define GAMMA_GAMMA_OBJECT_HPP

class GammaObject {
public:
  virtual ~GammaObject() {}
  virtual void update() = 0;
  virtual bool focused() = 0;
  virtual void on_click() = 0;
};
#endif
