#include "pch.h"
#include "init.h"
#include "buffer.h"
#include "font.h"
#include "console.h"
#include "interp.h"
#include "hotloader.h"
#include "input.h"


int main(int argc, char **argv) {
  if(Init_SDL()) return 1;
  init(argc, argv);

  Settings_Hotloader hotloader(settings_filename);

  while(!should_quit) {
    // measure_scope();

    SDL_Event e;
    if(SDL_PollEvent(&e)) {
      switch(e.type) {
        case SDL_QUIT:
          should_quit = true;
          break;

        case SDL_KEYDOWN:
          handle_input_keydown(e.key.keysym);
          break;

        case SDL_TEXTINPUT:
          if(is_insert_mode()) {
            get_current_buffer()->put(e.text.text[0]);
          } else if(is_console_mode()) {
            console_put(e.text.text[0]);
          }
          set_input();
          break;

        case SDL_WINDOWEVENT:
          if(e.window.event == SDL_WINDOWEVENT_RESIZED) {
            SDL_GetWindowSize(get_win(), &Width, &Height);
            for(auto tab : get_tabs()) { resize_tab(&tab); }
          }
          break;

        case SDL_MOUSEWHEEL: {
          auto buffer = get_current_buffer();

          if(e.wheel.y > 0) {
            for(char i = 0; i < dt_scroll; i++) {
              buffer->scroll_up();
            }
          } else if(e.wheel.y < 0) {
            for(char i = 0; i < dt_scroll; i++) {
              buffer->scroll_down();
            }
          }
          break;
        }

        default: break;
      }
    }


    // hotload settings.
    if(hotloader.settings_need_reload()) {
      hotloader.reload_file(settings_filename);
      update_variables();
      clear_font();
      make_font();
    }


    // rendering.
    if(!is_console_mode()) {
      draw_rect(0, 0, Width, get_console()->bottom_y, background_color);


      auto buffer = get_current_buffer();
      buffer->draw();  // @UpdateMultipleInstances: If given file is opened multiple times, we need to update them all.

      // Update cursor.
      char s = buffer->getchar();
      s = (s == '\n')? ' ': s;
      const int px = buffer->get_relative_pos_x(buffer->n_character - buffer->offset_on_line);
      const int py = buffer->get_relative_pos_y(buffer->n_line - buffer->start_pos);
      draw_text_shaded(get_font(), s, cursor_text_color, cursor_color, px, py);
    }

    draw_rect(0, get_console()->bottom_y, Width, font_height, console_color);
    console_draw();

    SDL_RenderPresent(get_renderer());
  }

  SDL_DestroyRenderer(get_renderer());
  SDL_DestroyWindow(get_win());
  TTF_CloseFont(get_font());
  TTF_Quit();
  SDL_Quit();
  return 0;
}
