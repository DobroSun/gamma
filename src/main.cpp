#include "pch.h"
#include "init.h"
#include "buffer.h"
#include "font.h"
#include "console.h"
#include "interp.h"

#include "our_string.h" 
#include "hotloader.h"




static const u8 Editor     = 0;
static const u8 Console    = 1;

static const u8 NormalMode = 0;
static const u8 InsertMode = 1;
static const u8 VisualMode = 2;

static u8   editor_state = Editor;
static u8   mode         = NormalMode;
static bool no_input     = false;





static void to_normal_mode() {
  mode = NormalMode;
  console_clear();
}

static void to_insert_mode() {
  mode     = InsertMode;
  no_input = true;
  console_put_text("-- INSERT --");
}

static void to_visual_mode() {
  mode = VisualMode;
  console_put_text("-- VISUAL --");
}


static void open_console() { 
  editor_state = Console;
  console_clear(); 
}

static void close_console() {
  editor_state = Editor;
}

static void no_mod_keydown(int key) {
  switch(mode) {
  case NormalMode:
    switch(key) {
    case 'a':
      get_current_buffer()->go_right();
      to_insert_mode();
      return;
    case 'h': get_current_buffer()->go_left();  return; 
    case 'l': get_current_buffer()->go_right(); return;
    case 'j': get_current_buffer()->go_down();  return;
    case 'k': get_current_buffer()->go_up();    return; 
    case 'x': get_current_buffer()->put_delete(); return;
    case 'i': to_insert_mode(); return;
    case 'v': to_visual_mode(); return;
    case '/': open_console();   return;
    case SDLK_ESCAPE:    should_quit = true; return;
    default: return;
    }

  case InsertMode:
    switch(key) {
    case SDLK_RETURN:    get_current_buffer()->put_return(); return;
    case SDLK_DELETE:    get_current_buffer()->put_delete(); return;
    case SDLK_BACKSPACE: get_current_buffer()->put_backspace(); return;
    case SDLK_ESCAPE: to_normal_mode(); return;
    default: return;
    }

  case VisualMode:
    switch(key) {
    case 'v':
    case SDLK_ESCAPE: to_normal_mode(); return;
    default: return;
    }
  }
}

static void shifted_keydown(int key) {
  switch(key) {
  case ';': open_console(); return; // ':'
  default: return;
  }
}

static void no_mod_console(int key) {
  switch(key) {
  case SDLK_ESCAPE:    close_console(); return;
  case SDLK_BACKSPACE: console_backspace(); return;
  case SDLK_DELETE:    console_del(); return;
  case SDLK_RETURN: 
    console_run_command(); 
    close_console(); 
    return;
  default: return;
  }
}

int main(int argc, char **argv) {
  if(Init_SDL()) return 1;
  init(argc, argv);

  Settings_Hotloader hotloader(settings_filename);

  while(!should_quit) {
    // measure_scope();

    SDL_Event e;
    if(SDL_PollEvent(&e)) {
      switch(e.type) {
        case SDL_QUIT: {
          should_quit = true;
        } break;

        case SDL_KEYDOWN: {
          auto key = e.key.keysym.sym;
          auto mod = e.key.keysym.mod;

          if(editor_state == Editor) {
            if(mod & KMOD_CTRL && mod & KMOD_SHIFT) {

            } else if(mod & KMOD_CTRL) {

            } else if(mod & KMOD_SHIFT) {
              shifted_keydown(key);

            } else {
              no_mod_keydown(key);
            }

          } else {
            assert(editor_state == Console);

            if(mod & (KMOD_CTRL | KMOD_SHIFT)) {
              // No special commands.
            } else {
              no_mod_console(key);
            }
          }
        } break;

        case SDL_TEXTINPUT: {
          switch(editor_state) {
          case Editor:
            if(mode != InsertMode) { break; }
            if(no_input)           { no_input = false; break; }
            get_current_buffer()->put(e.text.text[0]);
            break;

          case Console:
            console_put(e.text.text[0]);
            break;
          }
        } break;

        case SDL_WINDOWEVENT: {
          if(e.window.event == SDL_WINDOWEVENT_RESIZED) {
            SDL_GetWindowSize(get_win(), &Width, &Height);
            for(auto &tab : get_tabs()) { resize_tab(&tab); }
          }
        } break;

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
          } else {
            assert(0);
          }
        } break;

        default: {
        } break;
      }
    }

    // update.
    switch(editor_state) {
    case Editor: {
      draw_rect(0, 0, Width, get_console()->bottom_y, background_color);
      draw_tab(get_current_tab(), mode == VisualMode);
      SDL_RenderPresent(get_renderer());
      break;
    }

    case Console: {
      draw_rect(0, get_console()->bottom_y, Width, font_height, console_color);
      console_draw();
      SDL_RenderPresent(get_renderer());
      break;
    }
    }

    if(hotloader.settings_need_reload()) {
      hotloader.reload_file(settings_filename);

      update_variables();
      clear_font();
      make_font();
    }
  }

/* @MemoryLeak: Whatever.
  {
    auto &tabs = get_tabs();
    for(size_t k = 0; k < tabs.size; k++) {
      auto tab = &tabs[k];

      // @Incomplete:
      // If same file_buffer_t is used in different tabs,
      // This will cause double free of file_buffer_t,
      // Cause now we're freeing same buffers only for single tab.
      file_buffer_t *all_files[tab->buffers.size];
      size_t files_size = 0;

      for(size_t i = 0; i < tab->buffers.size; i++) {
        auto file_buffer = tab->buffers[i].file;
        if (!file_buffer) continue;

        // Check if file_buffer is already in all_files.
        bool already_in_files = false;
        for(size_t j = 0; j < files_size; j++) {
          if(file_buffer == all_files[j]) { already_in_files = true; break; }
        }
        // 

        if(already_in_files) {
          // Nothing.
        } else {
          all_files[files_size++] = file_buffer;
        }
      }

      for(size_t i = 0; i < files_size; i++) {
        auto file = all_files[i];

        for(size_t j = 0; j < file->undo.size; j++) { assert(file->undo[j].file); delete file->undo[j].file; }
        for(size_t j = 0; j < file->redo.size; j++) { assert(file->redo[j].file); delete file->redo[j].file; }
        delete file;
      }
    }

    for(size_t k = 0; k < tabs.size; k++) {
      auto tab = &tabs[k];
      for(size_t i = 0; i < tab->buffers.size; i++) {
        auto buffer = &tab->buffers[i];
        buffer->~buffer_t();
      }
      tab->~tab_t();
    }
  }
*/
  SDL_DestroyRenderer(get_renderer());
  SDL_DestroyWindow(get_win());
  TTF_CloseFont(get_font());
  TTF_Quit();
  SDL_Quit();
  return 0;
}
