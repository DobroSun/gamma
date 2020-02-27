#ifndef GAMMA_ACTION_STATE_HPP
#define GAMMA_ACTION_STATE_HPP

class GammaFactory;
class ActionState {
public:
  virtual ~ActionState() {}
  virtual bool handle_action() = 0;
};

class DefaultState: public ActionState {
public:
  bool handle_action() {
    bool continue_ = true;
    return continue_;
  }
};

class QuitState: public ActionState {
public:
  bool handle_action() {
    bool continue_ = false;
    return continue_;
  }
};

class ExitState: public ActionState {
  GammaFactory *fact;
public:
  ExitState(GammaFactory *fact_);
  ~ExitState();
  bool handle_action();
};
#endif
