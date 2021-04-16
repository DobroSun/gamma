#include "pch.h"
#include "init.h"
#include "buffer.h"
#include "font.h"
#include "console.h"
#include "interp.h"
#include "hotloader.h"


bool no_input = false;
void (*current_action)(buffer_t*);
void (*handle_keydown)(SDL_Keysym);



void open_console();
void close_console();

void handle_console_keydown(SDL_Keysym);
void handle_insert_mode_keydown(SDL_Keysym);
void handle_visual_mode_keydown(SDL_Keysym);
void handle_normal_mode_keydown(SDL_Keysym);


static bool is_normal_mode() { return handle_keydown == handle_normal_mode_keydown; }
static bool is_insert_mode() { return handle_keydown == handle_insert_mode_keydown; }
static bool is_visual_mode() { return handle_keydown == handle_visual_mode_keydown; }
static bool is_console_mode() { return handle_keydown == handle_console_keydown; }


void to_normal_mode() {
  handle_keydown = handle_normal_mode_keydown;
  console_clear();
}

void to_insert_mode() {
  no_input = true;
  handle_keydown = handle_insert_mode_keydown;
  console_put_text("-- INSERT --");
}

void to_visual_mode() {
  get_selection().first = get_current_buffer()->cursor;
  current_action = select_action;
  handle_keydown = handle_visual_mode_keydown;
  console_put_text("-- VISUAL --");
}

void open_console() { 
  handle_keydown = handle_console_keydown;
  console_clear(); 
}

void close_console() {
  handle_keydown = handle_normal_mode_keydown;
}

void handle_console_keydown(SDL_Keysym e) {
  switch(e.sym) {
  case SDLK_ESCAPE:    close_console(); break;
  case SDLK_BACKSPACE: console_backspace(); break;
  case SDLK_DELETE:    console_del(); break;
  case SDLK_RETURN: 
    console_run_command(); 
    close_console(); 
    break;
  default: break;
  }
}

void handle_insert_mode_keydown(SDL_Keysym e) {
  switch(e.sym) {
  case SDLK_LEFT:      get_current_buffer()->go_left();  break;
  case SDLK_RIGHT:     get_current_buffer()->go_right(); break;
  case SDLK_UP:        get_current_buffer()->go_up();    break;
  case SDLK_DOWN:      get_current_buffer()->go_down();  break;
  case SDLK_RETURN:    get_current_buffer()->put_return(); break;
  case SDLK_DELETE:    get_current_buffer()->put_delete(); break;
  case SDLK_BACKSPACE: get_current_buffer()->put_backspace(); break;
  case SDLK_TAB:       get_current_buffer()->put_tab(); break;
  case SDLK_ESCAPE:    to_normal_mode(); break;
  default: break;
  }
}

void handle_visual_mode_keydown(SDL_Keysym e) {
  switch(e.sym) {
  case 'h':
  case SDLK_LEFT:  get_current_buffer()->go_left();  break;
  case 'l':
  case SDLK_RIGHT: get_current_buffer()->go_right(); break;
  case 'k':
  case SDLK_UP:    get_current_buffer()->go_up();    break;
  case 'j':
  case SDLK_DOWN:  get_current_buffer()->go_down();  break;

  case 'v':
  case SDLK_ESCAPE: {
    auto &selection = get_selection();
    selection.first = 0;
    selection.last  = 0;
    current_action = no_action;
    to_normal_mode();
    break;
  }

  default: break;
  }
}

static bool is_modifing_key(int key) {
  switch(key) {
  case 'x':
  case 'i':
  case 'a':
    return true;
  default:
    return false;
  }
}

void handle_normal_mode_keydown(SDL_Keysym e) { 
  int key = e.sym;
  int mod = e.mod;

  if(is_modifing_key(key)) {
    save_current_state_for_undo(get_current_buffer());
  }

  if(mod & KMOD_SHIFT && mod & KMOD_CTRL) {
  } else if(mod & KMOD_CTRL) {
  } else if(mod & KMOD_SHIFT) {
    switch(key) {
    case 'a':                                  // 'A'
      go_to_end_of_line();
      to_insert_mode();
      break;
    case ';': open_console();      break;      // ':'
    case '4': go_to_end_of_line(); break;      // '$'
    case '[': go_paragraph_backwards(); break; // '{'
    case ']': go_paragraph_forward();   break; // '}'
    default: break;
    }

  } else { // no mods.
    switch(key) {
    case '0': go_to_beginning_of_line(); break;
    case 'w': go_word_forward();         break;
    case 'b': go_word_backwards();       break;

    case 'd': current_action = delete_action;
      break;
      
    case 'a':
      get_current_buffer()->go_right();
      to_insert_mode();
      break;

    case SDLK_BACKSPACE:
    case SDLK_LEFT: 
    case 'h':
      get_current_buffer()->go_left();  break; 
    case SDLK_SPACE:
    case SDLK_RIGHT: 
    case 'l':
      get_current_buffer()->go_right(); break;
    case SDLK_RETURN: 
    case SDLK_DOWN: 
    case 'j':
      get_current_buffer()->go_down();  break;
    case SDLK_UP: 
    case 'k':
      get_current_buffer()->go_up();    break; 
    case 'x': get_current_buffer()->put_delete(); break;
    case 'i': to_insert_mode();           break;
    case 'u': undo(get_current_buffer()); break; 
    case 'r': redo(get_current_buffer()); break;
    case 'v': to_visual_mode();           break;
    case '/': open_console();             break;
    case SDLK_ESCAPE: should_quit = true; break; // @Temporary: to_normal_mode();
    default: break;
    }
  }
}


int main(int argc, char **argv) {
  if(Init_SDL()) return 1;
  init(argc, argv);

  // Combine these into init(argc, argv);
  Settings_Hotloader hotloader(settings_filename);
  current_action = no_action;
  handle_keydown = handle_normal_mode_keydown;

  while(!should_quit) {
    measure_scope();

    SDL_Event e;
    if(SDL_PollEvent(&e)) {
      switch(e.type) {
        case SDL_QUIT:
          should_quit = true;
          break;

        case SDL_KEYDOWN:
          handle_keydown(e.key.keysym);
          break;

        case SDL_TEXTINPUT:
          if(is_insert_mode() && !no_input) { // @Hack: but whatever.
            get_current_buffer()->put(e.text.text[0]);
          } else if(is_console_mode()) {
            console_put(e.text.text[0]);
          }

          if(no_input) { no_input = false; }
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
          } else {
            assert(0);
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
    if(is_normal_mode() || is_insert_mode() || is_visual_mode()) {
      draw_rect(0, 0, Width, get_console()->bottom_y, background_color);


      auto buffer = get_current_buffer();
      // @UpdateMultipleInstances: If given file is opened multiple times, we need to update them all.
      buffer->draw(); 

      // Update cursor.
      char s = buffer->buffer[buffer->cursor];
      s = (s == '\n')? ' ': s;
      const int px = buffer->get_relative_pos_x(buffer->n_character - buffer->offset_on_line);
      const int py = buffer->get_relative_pos_y(buffer->n_line - buffer->start_pos);
      draw_text_shaded(get_font(), s, cursor_text_color, cursor_color, px, py);

    } else {
      assert(is_console_mode());
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
