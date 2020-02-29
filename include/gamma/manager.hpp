#ifndef GAMMA_MANAGER_HPP
#define GAMMA_MANAGER_HPP

union SDL_Event;
class SdlImpl;
class Manager {
public:
  virtual ~Manager() {}
  virtual void set_state(SdlImpl &sdl_impl, SDL_Event &event) = 0;
  virtual bool handle_action() = 0;
};

class ActionState;
class MainManager: public Manager {
  ActionState *state;
public:
  ~MainManager();
  MainManager();
  void set_state(SdlImpl &sdl_impl, SDL_Event &event);
  bool handle_action();
};

class ExitManager: public Manager {
  ActionState *state;
public:
  ~ExitManager();
  ExitManager();
  void set_state(SdlImpl &sdl_impl, SDL_Event &event);
  bool handle_action();
};
#endif
