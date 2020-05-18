#ifndef GAMMA_CACHE_H
#define GAMMA_CACHE_H
using String = std::vector<std::string>;

struct TextureCache {
  std::deque<SDL_Texture *> cached;
  char maxsize = 0;

  void push_right(SDL_Texture *p);
  void push_left(SDL_Texture *p);

  SDL_Texture *pop_right();
  SDL_Texture *pop_left();

  void remove_right();
  void remove_left();

  void set_maxsize(char other);
};

template<bool t>
void update_on_scroll(TextureCache &cache, const String &buffer, const int start);
#endif
