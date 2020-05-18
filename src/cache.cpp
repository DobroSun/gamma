#include "gamma/pch.h"
#include "gamma/cache.h"
#include "gamma/globals.h"
#include "gamma/utility.h"


void TextureCache::push_right(SDL_Texture *p) {
  if(cached.size() >= (unsigned)maxsize)
    remove_left();

  cached.push_back(p);
}

void TextureCache::push_left(SDL_Texture *p) {
  if(cached.size() >= (unsigned)maxsize)
    remove_right();

  cached.push_front(p);
}

void TextureCache::remove_right() {
  auto t = pop_right();
  SDL_DestroyTexture(t);
}

void TextureCache::remove_left() {
  auto t = pop_left();
  SDL_DestroyTexture(t);
}

SDL_Texture *TextureCache::pop_right() {
  auto b = cached.back();
  cached.pop_back();
  return b;
}

SDL_Texture *TextureCache::pop_left() {
  auto fr = cached.front();
  cached.pop_front();
  return fr;
}

void TextureCache::set_maxsize(char other) {
  maxsize = other;
}

template<>
void update_on_scroll<true>(TextureCache &cache, const String &buffer, const int start) {
  for(int k = 0; k < scroll_speed; k++) {
    if(start+numrows()+k == (int)buffer.size()) break;
    cache.push_right(load_normal(buffer[start+numrows()+k]));
  }
}

template<>
void update_on_scroll<false>(TextureCache &cache, const String &buffer, const int start) {
  for(int k = 0; k < scroll_speed; k++) {
    if(start == 0) break;
    cache.push_left(load_normal(buffer[start-k-1]));
  }
}
