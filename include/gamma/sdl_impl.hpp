#ifndef GAMMA_SDL_IMPLEMENTATION
#define GAMMA_SDL_IMPLEMENTATION
#include <SDL2/SDL.h>
#include <stdint.h>
#include <string>


struct SDL_Texture;
struct SDL_Rect;
class SdlImpl {
public:
  virtual ~SdlImpl() {}
  virtual bool poll_event(SDL_Event &event) = 0;
  virtual bool is_current_win(SDL_Event &event) = 0;
  virtual void set_draw_color(uint8_t r, uint8_t g,
                              uint8_t b, uint8_t a) = 0;
  virtual void show() = 0;
  virtual void get_win_size(int &w, int &h) = 0;
  virtual void render_rect(SDL_Texture *texture, const SDL_Rect *DstRect, const SDL_Rect *SrcRect=NULL) = 0;
  virtual SDL_Texture *make_texture(std::string &name) = 0;
};

struct SDL_Window;
struct SDL_Renderer;
class MainSdlImpl: public SdlImpl {
  SDL_Window *win;
  SDL_Renderer *renderer;
public:
  ~MainSdlImpl();
  MainSdlImpl();
  bool poll_event(SDL_Event &event);
  bool is_current_win(SDL_Event &event);
  void set_draw_color(uint8_t r, uint8_t g,
                      uint8_t b, uint8_t a);
  void show();
  void get_win_size(int &w, int &h);
  void render_rect(SDL_Texture *texture, const SDL_Rect *DstRect, const SDL_Rect *SrcRect=NULL);
  SDL_Texture *make_texture(std::string &name);
};

class ExitSdlImpl: public SdlImpl {
  SDL_Window *win;
  SDL_Renderer *renderer;
public:
  ~ExitSdlImpl();
  ExitSdlImpl();
  bool poll_event(SDL_Event &event);
  bool is_current_win(SDL_Event &event);
  void set_draw_color(uint8_t r, uint8_t g,
                      uint8_t b, uint8_t a);
  void show();
  void get_win_size(int &w, int &h);
  void render_rect(SDL_Texture *texture, const SDL_Rect *DstRect, const SDL_Rect *SrcRect=NULL);
  SDL_Texture *make_texture(std::string &name);
};
#endif
