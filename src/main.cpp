#include "pch.h"
#include "init.h"
#include "buffer.h"
#include "font.h"
#include "console.h"
#include "interp.h"
#include "input.h"

static bool render_button_with_text(const char *text, size_t size, int x, int y) {
  int width  = size * font_width;
  int height = font_height;

  draw_text_shaded(get_font(), text, BlackColor, WhiteColor, x, y);

  int mx, my;
  auto mask = SDL_GetMouseState(&mx, &my);

  if(mask & SDL_BUTTON(SDL_BUTTON_LEFT)) {
    if(x <= mx && mx <= x+width && y <= my && my <= y+height) {
      return true;
    }
  }
  return false;
}


// 
struct Settings_Hotloader {
  int fd;
  bool tries_to_update_second_time;
  const char *file;
};

Settings_Hotloader make_hotloader(const char *name) {
  int fd = inotify_init();

  if(fd == -1) { fprintf(stderr, "Inotify init failed!\n"); }

  int flags = fcntl(fd, F_GETFL, 0);
  fcntl(fd, F_SETFL, flags | O_NONBLOCK);

	int wd = inotify_add_watch(fd, name, IN_ALL_EVENTS);
  if(wd == -1) { fprintf(stderr, "Failed to add a watch for `syntax.m`\n"); }

  Settings_Hotloader r = {};
  r.fd   = fd;
  r.file = name;
  return r;
}

bool settings_need_reload(Settings_Hotloader *h) {
  inotify_event event;
  if(read(h->fd, &event, sizeof(event)) != -1) {
    if(event.mask & IN_MODIFY) {
      return true;
    }
  }
  return false;
}

void reload_file(Settings_Hotloader *h) {
  if(h->tries_to_update_second_time) {
    h->tries_to_update_second_time = false;
    return;
  }

  char *string = NULL;
  defer { if(string) deallocate(string); };

  if(FILE *f = fopen(h->file, "r")) {
    defer { fclose(f); };
    read_file_into_memory(f, &string);
  } else {
    return;
  }

  interp(string);
  h->tries_to_update_second_time = true;
}
// 

void init(int argc, char **argv, Settings_Hotloader *hotloader) {
  init_variable_table();

  string filename = {};
  if(argc > 1) {
    for(int i = 1; i < argc; i++) { // parsing command line arguments.
      const char *arg = argv[i];
      int len    = strlen(arg);
      int cursor = 0;

      literal option = {};
      if(arg[cursor] == '-') {
        cursor++;
        if(arg[cursor] == '-') {
          // It's a command line option.
          cursor++;
          const char *tmp = arg + cursor;
          while(arg[cursor] != '\0' && arg[cursor] != '=') {
            cursor++;
          }
          option.data = tmp;
          option.size = cursor - 2; // `--`.
        } else {
          report_error("Error: use `--` for command line options.\n");
          continue;
        }
      } else {
        // It's a positional argument.
        if(!filename.size) filename = to_string(arg, len);
      }

      if(option == "settings") {
        if(arg[cursor++] != '=') {
          report_error("Error: `settings` option expects a path to settings file.\n", settings_filename);
          continue;
        }
        if(arg[cursor] == '\0') {
          report_error("Error: `settings` option requires non null path.\n");
          continue;
        }
        settings_filename = arg + cursor;
        // done.

      } else { // other options.
      }
    }
  } else {
    // No command line arguments provided.
  }

  *hotloader = make_hotloader(settings_filename);
  reload_file(hotloader);

  update_variables();
  make_font();

  open_new_tab(filename);
}



static void deallocate_everything(Settings_Hotloader *h) {
  auto tabs = get_tabs();

  for(auto &tab : tabs) {
    for(auto &buffer : tab.buffers) {
      finish_buffer(&buffer);
    }
    free_array(&tab.buffers);
  }
  free_array(&tabs);

  finish_copy();

  {
    auto console = get_console();
    free_gap_buffer(&console->buffer);
  }
  
  finish_settings();

  close(h->fd);
}


int main(int argc, char **argv) {
  if(Init_SDL()) return 1;

  Settings_Hotloader hotloader;
  init(argc, argv, &hotloader);


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
    if(settings_need_reload(&hotloader)) {
      reload_file(&hotloader);
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
              // Rendering intermediate lines.
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

    // draw console.
    int y = get_console()->bottom_y;
    {
      draw_rect(0, y, Width, font_height, console_color);
      console_draw();
    }

    {
      // draw tab panel.
      y -= font_height;
      auto tabs = get_tabs();
      int  x = 0;

      draw_rect(x, y, Width, font_height, WhiteColor);

      For(tabs) {
        string filename = it->buffers[0].filename;
        if(!filename.size) break;
        
        char str[filename.size+2+1] = {};
        str[0] = it_index + '0';
        str[1] = ':';
        memcpy(str + 2, filename.data, filename.size);

        bool pressed = render_button_with_text(str, array_size(str), x, y);
        if(pressed) { change_tab(it_index); }

        x += array_size(str) * font_width;
      }
    }

    // render.
    SDL_RenderPresent(get_renderer());
  }

  deallocate_everything(&hotloader);
  report_all_memory_leaks();

  /*
  SDL_DestroyRenderer(get_renderer());
  SDL_DestroyWindow(get_win());
  TTF_CloseFont(get_font());
  TTF_Quit();
  SDL_Quit();
  */
  return 0;
}
