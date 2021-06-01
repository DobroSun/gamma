#include "pch.h"
#include "input.h"
#include "buffer.h"
#include "console.h"


bool no_input = false; // When we switch from normal mode to insert mode, it immediately puts `i` on screen, but we want it to wait untill next keydown.
void set_input() { no_input = false; }


void (*current_action)(Buffer_Component*)  = no_action;
void (*handle_keydown)(SDL_Keysym) = handle_normal_mode_keydown;

void handle_input_keydown(SDL_Keysym k) { handle_keydown(k); }


bool is_normal_mode()  { return handle_keydown == handle_normal_mode_keydown; }
bool is_insert_mode()  { return handle_keydown == handle_insert_mode_keydown && !no_input; }
bool is_console_mode() { return handle_keydown == handle_console_keydown; }
bool is_tab_mode()     { return handle_keydown == handle_tab_mode_keydown; }

// @Note: Since visual mode needs to be able to move cursor(i.e. reuse normal mode moving functions) we don't create new mode for that.
bool is_visual_mode()      { return current_action == select_action || current_action == select_to_left || current_action == select_to_right; }
bool is_visual_line_mode() { return current_action == select_line_action || current_action == select_line_to_left || current_action == select_line_to_right; }


void to_normal_mode() {
  current_action = no_action;
  handle_keydown = handle_normal_mode_keydown;
  console_clear();
}

void to_insert_mode() { // @Incomplete: When we are in visual mode we shouldn't be able to change it to insert mode directly.
  no_input = true;
  handle_keydown = handle_insert_mode_keydown;
  update_indentation_level(get_current_buffer());
  console_put_text("-- INSERT --");
}

void to_visual_mode() {
  assert(handle_keydown == handle_normal_mode_keydown);
  get_selection().first = get_current_buffer()->buffer_component.cursor();
  get_selection().last  = get_selection().first;
  current_action = select_action;
  console_put_text("-- VISUAL --");
}

void to_visual_line_mode() {
  assert(handle_keydown == handle_normal_mode_keydown);

  Buffer_Component t = get_current_buffer()->buffer_component;
  t.cursor1 = t.cursor();
  get_selection().first = to_beginning_of_line(t).cursor1;
  get_selection().last  = to_end_of_line(t).cursor1;
  current_action = select_line_action;
  console_put_text("-- VISUAL LINE --");
}

void to_tab_mode() {
  handle_keydown = handle_tab_mode_keydown;
  console_clear();
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
  int key = e.sym;
  int mod = e.mod;

  if(mod & KMOD_CTRL) {
    switch(key) {
    case 'c': console_clear(); break;
    default: break;
    }
  } else {
    switch(key) {
    case SDLK_ESCAPE:
      console_clear(); 
      close_console();
      break;
    case SDLK_BACKSPACE: console_backspace(); break;
    case SDLK_DELETE:    console_del(); break;
    case SDLK_RETURN: 
      console_run_command(); 
      close_console(); 
      break;
    default: break;
    }
  }
}

void handle_insert_mode_keydown(SDL_Keysym e) {
  switch(e.sym) {
  case SDLK_LEFT:      get_current_buffer()->buffer_component.go_left();  break;
  case SDLK_RIGHT:     get_current_buffer()->buffer_component.go_right(); break;
  case SDLK_UP:        get_current_buffer()->buffer_component.go_up();    break;
  case SDLK_DOWN:      get_current_buffer()->buffer_component.go_down();  break;
  case SDLK_RETURN:    get_current_buffer()->buffer_component.put_return(); break;
  case SDLK_DELETE:    get_current_buffer()->buffer_component.put_delete(); break;
  case SDLK_BACKSPACE: get_current_buffer()->buffer_component.put_backspace(); break;
  case SDLK_TAB:       get_current_buffer()->buffer_component.put_tab(); break;
  case SDLK_ESCAPE:    to_normal_mode(); break;
  default: break;
  }
}

void handle_tab_mode_keydown(SDL_Keysym e) {
  if(isdigit(e.sym)) {
    change_tab(e.sym - '0');
  }

  switch(e.sym) {
  case 'c':
    open_new_tab(to_string("console.cpp")); // @Temoprary:
    break;
  }

  to_normal_mode();
}

static bool is_modifying_key(int key) {
  switch(key) {
  case 'd':
  case 'p':
  case 'x':
  case 'o':
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

  if(is_modifying_key(key)) {
    save_current_state_for_undo(&get_current_buffer()->undo_component, &get_current_buffer()->buffer_component);
  }

  if(mod & KMOD_SHIFT && mod & KMOD_CTRL) {
    // CTRL + SHIFT 
  } else if(mod & KMOD_CTRL) {
    // CTRL
  } else if(mod & KMOD_SHIFT) {
    // SHIFT + SHARED TO ALL MODS
    switch(key) { 
    case ';': open_console();      break;      // ':'
    case '4': get_current_buffer()->buffer_component = move_to(to_end_of_line(get_current_buffer()->buffer_component)); break; // '$'
    case '[': go_paragraph_backwards(); break; // '{'
    case ']': go_paragraph_forward();   break; // '}'
    default: break;
    }

    if(is_visual_mode()) {
      // SHIFT + VISUAL

    } else if(is_visual_line_mode()) {
      // SHIFT + VISUAL LINE
      switch(key) {
      case 'v': to_normal_mode(); break; // 'V'.
      default: break;
      }

    } else {
      // SHIFT + NORMAL
      switch(key) {
      case 'v': to_visual_line_mode(); break; // 'V'.
      case 'd': {   // 'D'
        auto buffer = get_current_buffer();
        current_action = delete_action;
        buffer->buffer_component = move_to(to_end_of_line(buffer->buffer_component));
        break;
      }
      case 'a':     // 'A'
        get_current_buffer()->buffer_component = move_to(to_end_of_line(get_current_buffer()->buffer_component));
        to_insert_mode();
        break;
      default: break;
      }
    }

  } else {
    // SHARED TO ALL MODS
    switch(key) {
    case '0': get_current_buffer()->buffer_component = move_to(to_beginning_of_line(get_current_buffer()->buffer_component)); break;
    case 'w': go_word_forward();        break;
    case 'b': go_word_backwards();      break;

    case SDLK_BACKSPACE:
    case SDLK_LEFT: 
    case 'h':
      get_current_buffer()->buffer_component.go_left();  break; 
    case ' ':
    case SDLK_RIGHT: 
    case 'l':
      get_current_buffer()->buffer_component.go_right(); break;
    case SDLK_RETURN: 
    case SDLK_DOWN: 
    case 'j':
      get_current_buffer()->buffer_component.go_down();  break;
    case SDLK_UP: 
    case 'k':
      get_current_buffer()->buffer_component.go_up();    break; 

    case '/': open_console();           break;
    case 'n': to_next_in_search(&get_current_buffer()->search_component, &get_current_buffer()->buffer_component); break;
    case 'm': to_prev_in_search(&get_current_buffer()->search_component, &get_current_buffer()->buffer_component); break;
    default: break;
    }

    if(is_visual_mode()) {
      // VISUAL
      switch(key) {
      case 'y':
        yield_selected(get_current_buffer()->buffer_component.buffer, get_selection());
        to_normal_mode();
        break; 
      case 'd':
        delete_selected(get_current_buffer());
        to_normal_mode();
        break; 

      case 'x': /* ... */         break;
      case 'v': to_normal_mode(); break;
      case SDLK_ESCAPE:
        current_action = no_action;
        to_normal_mode();
        break;

      default: break;
      }
    } else if(is_visual_line_mode()) {
      // VISUAL LINE
      switch(key) {
      case 'y':
        yield_selected(get_current_buffer()->buffer_component.buffer, get_selection());
        to_normal_mode();
        break; 
      case 'd':
        delete_selected(get_current_buffer());
        to_normal_mode();
        break; 
      case SDLK_ESCAPE: to_normal_mode(); break;
      default: break;
      }
    } else {
      // NORMAL
      switch(key) {
      case '`':
        to_tab_mode();
        break;

      case 'a':
        get_current_buffer()->buffer_component.go_right();
        to_insert_mode();
        break;

      case 'y':
        if(current_action == yield_action) { // 'yy'.
          auto buffer = get_current_buffer();
          auto select = get_selection();

          select.first = to_beginning_of_line(buffer->buffer_component).cursor1;
          select.last  = to_end_of_line(buffer->buffer_component).cursor1;

          yield_selected(buffer->buffer_component.buffer, select);
          to_normal_mode();
        } else {
          current_action = yield_action;
        }
        break;

      case 'd':
        if(current_action == delete_action) { // 'dd'.
          auto buffer = get_current_buffer();
          buffer->buffer_component = move_to(to_end_of_line(buffer->buffer_component));
          current_action = delete_action;
          buffer->buffer_component = move_to(to_beginning_of_line(buffer->buffer_component));
          buffer->buffer_component.put_delete();
          to_normal_mode();
        } else {
          current_action = delete_action;
        }
        break; 

      case 'o': 
        get_current_buffer()->buffer_component = move_to(to_end_of_line(get_current_buffer()->buffer_component));
        get_current_buffer()->buffer_component.put_return();
        to_insert_mode();
        break;

      case 'p': paste_from_buffer(&get_current_buffer()->buffer_component); break;
      case 'x': get_current_buffer()->buffer_component.put_delete();      break;
      case 'i': to_insert_mode();           break;
      case 'u': undo(&get_current_buffer()->undo_component, &get_current_buffer()->buffer_component); break;
      case 'r': redo(&get_current_buffer()->undo_component, &get_current_buffer()->buffer_component); break;
      case 'v': to_visual_mode();           break;
      case SDLK_ESCAPE: should_quit = true; break; // @Temporary: 

      default: break;
      }
    }
  }
}

