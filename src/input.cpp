#include "pch.h"
#include "input.h"
#include "buffer.h"
#include "console.h"

static void no_action(buffer_t*) {}


bool no_input = false; // When we switch from normal mode to insert mode, it immediately puts `i` on screen, but we want it to wait untill next keydown.
void set_input() { no_input = false; }


void (*current_action)(buffer_t*)  = no_action;
void (*handle_keydown)(SDL_Keysym) = handle_normal_mode_keydown;

void handle_input_keydown(SDL_Keysym k) { handle_keydown(k); }


bool is_normal_mode()  { return handle_keydown == handle_normal_mode_keydown; }
bool is_insert_mode()  { return handle_keydown == handle_insert_mode_keydown && !no_input; }
bool is_console_mode() { return handle_keydown == handle_console_keydown; }
bool is_visual_mode() { return current_action == select_action || current_action == select_to_left || current_action == select_to_right; }


void to_normal_mode() {
  current_action = no_action;
  handle_keydown = handle_normal_mode_keydown;
  console_clear();
}

void to_insert_mode() { // @Incomplete: When we are in visual mode we shouldn't be able to change it to insert mode directly.
  no_input = true;
  handle_keydown = handle_insert_mode_keydown;
  console_put_text("-- INSERT --");
}

void to_visual_mode() {
  assert(handle_keydown == handle_normal_mode_keydown);
  get_selection().first = get_current_buffer()->cursor();
  get_selection().last  = get_selection().first;
  current_action = select_action;
  console_put_text("-- VISUAL --");
}

void open_console() { 
  handle_keydown = handle_console_keydown;
  console_clear(); 
}

void close_console() {
  handle_keydown = handle_normal_mode_keydown;
  current_action = no_action;
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
      get_current_buffer()->go_to_index(get_current_buffer()->to_end_of_line(get_current_buffer()->cursor()));
      to_insert_mode();
      break;
    case ';': open_console();      break;      // ':'
    case '4': get_current_buffer()->go_to_index(get_current_buffer()->to_end_of_line(get_current_buffer()->cursor())); break; // '$'
    case '[': go_paragraph_backwards(); break; // '{'
    case ']': go_paragraph_forward();   break; // '}'
    default: break;
    }

  } else { // no mods.
    switch(key) {
    case '0': get_current_buffer()->go_to_index(get_current_buffer()->to_beginning_of_line(get_current_buffer()->cursor())); break;
    case 'w': go_word_forward();         break;
    case 'b': go_word_backwards();       break;

#if 0
    case 'd': {
      auto select = get_selection();
      auto buffer = get_current_buffer();

      for(size_t i = select.first; i <= select.last; i++) {
        printf("%c", buffer->buffer[i]);
      }
      printf("\n");
      break;
    }
#endif

    case 'a':
      get_current_buffer()->go_right();
      to_insert_mode();
      break;

    case SDLK_BACKSPACE:
    case SDLK_LEFT: 
    case 'h':
      get_current_buffer()->go_left();  break; 
    case ' ':
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

    case 'v':
      if(is_visual_mode()) {
        to_normal_mode();
      } else {
        to_visual_mode();
      }
      break;

    case '/': open_console();             break;
    case 'n': to_next_in_search();        break;
    case 'm': to_prev_in_search();        break;

    case SDLK_ESCAPE:
      if(is_visual_mode()) {
        auto &selection = get_selection();
        selection.first = 0;
        selection.last  = 0;
        current_action = no_action;
        to_normal_mode();
      } else {
        should_quit = true; // @Temporary: 
      }
     break;

    default: break;
    }
  }
}

