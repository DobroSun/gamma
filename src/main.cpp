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
    if(SDL_WaitEvent(&e)) {
      switch(e.type) {
        case SDL_QUIT:
          should_quit = true;
          break;

        case SDL_KEYDOWN:
          handle_input_keydown(e.key.keysym);
          break;

        case SDL_TEXTINPUT:
          if(is_insert_mode()) {
            get_current_buffer()->buffer_component.put(e.text.text[0]);
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
              buffer->buffer_component.scroll_up();
            }
          } else if(e.wheel.y < 0) {
            for(char i = 0; i < dt_scroll; i++) {
              buffer->buffer_component.scroll_down();
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


    if(!is_console_mode()) {
      draw_rect(0, 0, Width, get_console()->bottom_y, background_color);

      auto  buffer    = get_current_buffer();
      auto  selection = get_selection();
      auto &buffer_component = buffer->buffer_component;
      buffer->draw();  // @UpdateMultipleInstances: If given file is opened multiple times, we need to update them all.

      // Update cursor.
      char s = buffer_component.getchar();
      s = (s == '\n')? ' ': s;
      const int px = buffer_component.get_relative_pos_x(buffer_component.n_character - buffer->offset_on_line);
      const int py = buffer_component.get_relative_pos_y(buffer_component.n_line - buffer_component.start_pos);
      draw_text_shaded(get_font(), s, cursor_text_color, cursor_color, px, py);

      if(is_visual_mode() || is_visual_line_mode()) {
        // @CleanUp: @Copy&Paste: from buffer_t::draw().
        size_t y = buffer_component.get_relative_pos_y(0);
        size_t i = buffer_component.offset_from_beginning;

        bool already_rendering_selected_text = false;
        while(i < buffer_component.buffer.size()) {
          int current_line_length = buffer_component.get_line_length(i);

          if(y >= get_console()->bottom_y - font_height) { break; }

          if(!already_rendering_selected_text) {
            if(i <= selection.first && selection.first <= i + current_line_length-1) {
              already_rendering_selected_text = true;

              // Rendering first line.
              size_t end  = min(selection.last, i+current_line_length-1);
              size_t size = end-selection.first+1;
              char string[size+1] = {};

              int x = buffer_component.get_relative_pos_x(-buffer->offset_on_line + selection.first-i);
              for(size_t j = selection.first; j <= end; j++) {
                char c = buffer->buffer_component.buffer[j];
                string[j-selection.first] = (c == '\n') ? ' ' : c;
              }
              draw_text_shaded(get_font(), string, cursor_text_color, cursor_color, x, y);
              if(end == selection.last) break;
              // 
            }
          } else {
            if(i <= selection.last && selection.last <= i+current_line_length-1) {
              // Rendering last line.
              char string[selection.last+1-i+1] = {};
              int x = buffer_component.get_relative_pos_x(-buffer->offset_on_line);
              for(size_t j = i; j <= selection.last; j++) {
                char c = buffer->buffer_component.buffer[j];
                string[j-i] = (c == '\n') ? ' ' : c;
              }
              draw_text_shaded(get_font(), string, cursor_text_color, cursor_color, x, y);
              break;

            } else {
              // Rendering intermediate line.
              char string[current_line_length+1] = {};
              int x = buffer_component.get_relative_pos_x(-buffer->offset_on_line);

              for(size_t j = i; j <= i+current_line_length-1; j++) {
                char c = buffer->buffer_component.buffer[j];
                string[j-i] = (c == '\n') ? ' ' : c;
              }
              draw_text_shaded(get_font(), string, cursor_text_color, cursor_color, x, y);
            }
          }

          y += font_height;
          i += current_line_length;
        }
      }
    }

    draw_rect(0, get_console()->bottom_y, Width, font_height, console_color);
    console_draw();

    SDL_RenderPresent(get_renderer());
  }

  /*
  SDL_DestroyRenderer(get_renderer());
  SDL_DestroyWindow(get_win());
  TTF_CloseFont(get_font());
  TTF_Quit();
  SDL_Quit();
  */
  return 0;
}
