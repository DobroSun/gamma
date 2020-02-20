#ifndef GAMMA_MANAGER_HPP
#define GAMMA_MANAGER_HPP
#include <inttypes.h>

class Manager {
public:
  virtual ~Manager() {}
  virtual void set_state(uint32_t e_type) = 0;
  virtual bool handle_action() = 0;
};

class ActionState;
class MainManager: public Manager {
  ActionState *state;
public:
  ~MainManager();
  MainManager();
  void set_state(uint32_t e_type);
  bool handle_action();
};
#endif
