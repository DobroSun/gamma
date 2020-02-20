#ifndef GAMMA_ACTION_STATE_HPP
#define GAMMA_ACTION_STATE_HPP

class ActionState {
public:
  virtual ~ActionState() {}
  virtual bool handle_action() = 0;
};

class DefaultState: public ActionState {
public:
  bool handle_action() {
    return true;
  }
};

class ExitState: public ActionState {
public:
  bool handle_action();
};
#endif
