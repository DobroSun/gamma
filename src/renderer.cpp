#include "gamma/renderer.hpp"
#include <SDL2/SDL.h>

#include <cassert>

RendererImpl::RendererImpl(SDL_Window *win) {
}

RendererImpl::~RendererImpl() {
  SDL_DestroyRenderer(sdl_renderer);
}

void RendererImpl::set_color(int a, int b, int c, int d) {
  SDL_SetRenderDrawColor(sdl_renderer, a, b, c, d);
}

void RendererImpl::copy(SDL_Texture *texture, SDL_Rect *SrcR, SDL_Rect *DestR) {
  SDL_RenderCopy(sdl_renderer, texture, SrcR, DestR);
}

void RendererImpl::present() {
  SDL_RenderPresent(sdl_renderer);
}

void RendererImpl::clear() {
  SDL_RenderClear(sdl_renderer);
}
