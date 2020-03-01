#ifndef GAMMA_TESTS_MOCK_SDLIMPL_HPP
#define GAMMA_TESTS_MOCK_SDLIMPL_HPP
#include "gamma/sdl_impl.hpp"
#include "gmock/gmock.h"
#include <stdint.h>
#include <string>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

class MockSdlImpl: public SdlImpl {
public:
  MOCK_METHOD(bool, poll_event, (SDL_Event &event), (override));
  MOCK_METHOD(bool, is_current_win, (SDL_Event &event), (override));
  MOCK_METHOD(void, set_draw_color, (uint8_t r, uint8_t g, uint8_t b, uint8_t a), (override));
  MOCK_METHOD(void, show, (), (override));
  MOCK_METHOD(void, get_win_size, (int &w, int &h), (override));
  MOCK_METHOD(void, render_rect, (SDL_Texture *texture, const SDL_Rect *Srect, const SDL_Rect *Drect), (override));
  MOCK_METHOD(SDL_Texture*, make_texture, (std::string &path), (override));
};
#endif
