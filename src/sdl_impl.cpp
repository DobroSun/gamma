#include "gamma/sdl_impl.hpp"
#include "gamma/globals.hpp"

#include <cassert>
#include <stdint.h>

#include <SDL2/SDL_image.h>

// FIXME:
// Somehow refactor copying methods.
MainSdlImpl::MainSdlImpl() {
  uint32_t flags = SDL_WINDOW_RESIZABLE;
  win = SDL_CreateWindow("",
              SDL_WINDOWPOS_CENTERED,
              SDL_WINDOWPOS_CENTERED,
              WIDTH, HEIGHT, flags);
  renderer = SDL_CreateRenderer(win, -1, 0);
  
  assert(win && "Couldn't create window");
  assert(renderer && "Couldn't create renderer");
}

MainSdlImpl::~MainSdlImpl() {
  assert(win && "Window already doesnt' exists, cannot delete");
  assert(renderer && "Renderer already doesnt' exists, cannot delete");
  SDL_DestroyWindow(win);
  SDL_DestroyRenderer(renderer);
}

bool MainSdlImpl::poll_event(SDL_Event &event) {
  return SDL_PollEvent(&event);
}

bool MainSdlImpl::is_current_win(SDL_Event &event) {
  return (SDL_GetWindowID(win) == event.window.windowID);
};

void MainSdlImpl::set_draw_color(uint8_t r, uint8_t g,
                    uint8_t b, uint8_t a) {
  SDL_SetRenderDrawColor(renderer, r, g, b, a);
}

void MainSdlImpl::show() {
  SDL_RenderPresent(renderer);
  SDL_RenderClear(renderer);
}

void MainSdlImpl::get_win_size(int &w, int &h) {
  SDL_GetWindowSize(win, &w, &h);
}

void MainSdlImpl::render_rect(SDL_Texture *texture, const SDL_Rect *DstRect, const SDL_Rect *SrcRect) {
  SDL_RenderCopy(renderer, texture, NULL, DstRect);
}

SDL_Texture *MainSdlImpl::make_texture(std::string &path) {
  SDL_Surface *image = IMG_Load((GAMMA_ASSETS_PATH+path).c_str());
  SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, image);
  SDL_FreeSurface(image);
  return texture;
}

ExitSdlImpl::ExitSdlImpl() {
  uint32_t flags = 0;
  win = SDL_CreateWindow("Are you sure want to exit?",
                  SDL_WINDOWPOS_CENTERED,
                  SDL_WINDOWPOS_CENTERED,
                  EXITWIDTH, EXITHEIGHT, flags);
  renderer = SDL_CreateRenderer(win, -1, 0);
  
  assert(win && "Couldn't create window");
  assert(renderer && "Couldn't create renderer");
};

ExitSdlImpl::~ExitSdlImpl() {
  assert(win && "Window already doesnt' exists, cannot delete");
  assert(renderer && "Renderer already doesnt' exists, cannot delete");
  SDL_DestroyWindow(win);
  SDL_DestroyRenderer(renderer);
}

bool ExitSdlImpl::poll_event(SDL_Event &event) {
  return SDL_PollEvent(&event);
}

bool ExitSdlImpl::is_current_win(SDL_Event &event) {
  return (SDL_GetWindowID(win) == event.window.windowID);
}

void ExitSdlImpl::set_draw_color(uint8_t r, uint8_t g,
                    uint8_t b, uint8_t a) {
  SDL_SetRenderDrawColor(renderer, r, g, b, a);
}

void ExitSdlImpl::show() {
  SDL_RenderPresent(renderer);
  SDL_RenderClear(renderer);
}

void ExitSdlImpl::get_win_size(int &w, int &h) {
  SDL_GetWindowSize(win, &w, &h);
}

void ExitSdlImpl::render_rect(SDL_Texture *texture, const SDL_Rect *DstRect, const SDL_Rect *SrcRect) {
  SDL_RenderCopy(renderer, texture, NULL, DstRect);
}

SDL_Texture *ExitSdlImpl::make_texture(std::string &path) {
  SDL_Surface *image = IMG_Load((GAMMA_ASSETS_PATH+path).c_str());
  SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, image);
  SDL_FreeSurface(image);
  return texture;
}
