#include "pch.h"
#include "init.h"
#include "buffer.h"
#include "font.h"
#include "console.h"

extern "C" {
#include "lua/include/lua.h"
#include "lua/include/lauxlib.h"
#include "lua/include/lualib.h"
}


const int Editor  = 0;
const int Console = 1;

const int NormalMode = 0;
const int InsertMode = 1;
const int VisualMode = 2;


static bool check_lua(lua_State *L, int r) {
  bool ok = (r == LUA_OK);
  if(!ok) {
    print(lua_tostring(L, -1));
    should_quit = true;
  }
  return ok;
}

static int save(lua_State *L) {
  save();
  return 0;
}

static int put(lua_State *L) {
  const char *c = lua_tostring(L, 1);
  get_current_buffer()->put(c[0]);
  return 0;
}

static int put_return(lua_State *L) {
  get_current_buffer()->put_return();
  return 0;
}

static int put_backspace(lua_State *L) {
  get_current_buffer()->put_backspace();
  return 0;
}

static int put_delete(lua_State *L) {
  get_current_buffer()->put_delete();
  return 0;
}

static int go_right(lua_State *L) {
  bool with_selection = lua_toboolean(L, 1);
  get_current_buffer()->go_right(with_selection);
  return 0;
}

static int go_left(lua_State *L) {
  bool with_selection = lua_toboolean(L, 1);
  get_current_buffer()->go_left(with_selection);
  return 0;
}

static int go_up(lua_State *L) {
  bool with_selection = lua_toboolean(L, 1);
  get_current_buffer()->go_up(with_selection);
  return 0;
}

static int go_down(lua_State *L) {
  bool with_selection = lua_toboolean(L, 1);
  get_current_buffer()->go_down(with_selection);
  return 0;
}

static int cursor_right(lua_State *L) {
  bool with_selection = lua_toboolean(L, 1);
  cursor_right(with_selection);
  return 0;
}

static int cursor_left(lua_State *L) {
  bool with_selection = lua_toboolean(L, 1);
  cursor_left(with_selection);
  return 0;
}

static int to_beginning_of_line(lua_State *L) {
  bool with_selection = lua_toboolean(L, 1);
  to_beginning_of_line(with_selection);
  return 0;
}

static int to_end_of_line(lua_State *L) {
  bool with_selection = lua_toboolean(L, 1);
  to_end_of_line(with_selection);
  return 0;
}

static int go_word_forward(lua_State *L) {
  bool with_selection = lua_toboolean(L, 1);
  go_word_forward(with_selection);
  return 0;
}

static int go_word_backwards(lua_State *L) {
  bool with_selection = lua_toboolean(L, 1);
  go_word_backwards(with_selection);
  return 0;
}

static int delete_selected(lua_State *L) {
  delete_selected();
  return 0;
}

static int start_selection(lua_State *L) {
  auto selected = get_selection_buffer();
  auto buffer   = get_current_buffer(); 

  selected->start_index = buffer->cursor;
  selected->start_line  = buffer->n_line;
  selected->start_char  = buffer->n_character;
  selected->size        = 0;
  selected->direction   = none;
  return 0;
}

static int copy_selected(lua_State *L) {
  copy_selected();
  return 0;
}

static int paste_from_global(lua_State *L) {
  paste_from_global_copy();
  return 0;
}

static int console_clear(lua_State *L) {
  console_clear();
  return 0;
}

static int console_put(lua_State *L) {
  const char *s = lua_tostring(L, 1);
  console_put(s[0]);
  return 0;
}

static int console_put_text(lua_State *L) {
  const char *t = lua_tostring(L, 1);
  console_put_text(t);
  return 0;
}

static int console_put_delete(lua_State *L) {
  console_del();
  return 0;
}

static int console_put_backspace(lua_State *L) {
  console_backspace();
  return 0;
}

static int console_go_right(lua_State *L) {
  console_go_right();
  return 0;
}

static int console_go_left(lua_State *L) {
  console_go_left();
  return 0;
}

static int console_eval(lua_State *L) {
  console_run_command();
  return 0;
}

static int quit(lua_State *L) {
  should_quit = true;
  return 0;
}

static void interp_lua_table(lua_State *L, const char *name, int key) {
  const char b[] = { (char)key, '\0' };
  lua_getglobal(L, name);
  if(lua_istable(L, -1)) {
    lua_pushstring(L, b);
    lua_gettable(L, -2);

    if(lua_isfunction(L, -1)) {
      if(check_lua(L, lua_pcall(L, 0, 0, 0))) {
      }
    }
  }
}

int main(int argc, char **argv) {
  if(Init_SDL()) return 1;
  init(argc, argv);

  lua_State *L = luaL_newstate();
  lua_register(L, "save", &save);
  lua_register(L, "put",  &put);
  lua_register(L, "quit", &quit);
  lua_register(L, "go_right", &go_right);
  lua_register(L, "go_left",  &go_left);
  lua_register(L, "cursor_right", &cursor_right);
  lua_register(L, "cursor_left",  &cursor_left);
  lua_register(L, "go_down",  &go_down);
  lua_register(L, "go_up",    &go_up);
  lua_register(L, "to_beginning_of_line", &to_beginning_of_line);
  lua_register(L, "to_end_of_line", &to_end_of_line);
  lua_register(L, "go_word_forward", &go_word_forward);
  lua_register(L, "go_word_backwards", &go_word_backwards);
  lua_register(L, "put_return", &put_return);
  lua_register(L, "put_backspace", &put_backspace);
  lua_register(L, "put_delete", &put_delete);
  lua_register(L, "delete_selected", &delete_selected);
  lua_register(L, "start_selection", &start_selection);
  lua_register(L, "copy_selected", &copy_selected);
  lua_register(L, "paste_from_global", &paste_from_global);
  lua_register(L, "console_put_text", &console_put_text);
  lua_register(L, "console_put", &console_put);
  lua_register(L, "console_clear", &console_clear);
  lua_register(L, "console_put_backspace", &console_put_backspace);
  lua_register(L, "console_put_delete", &console_put_delete);
  lua_register(L, "console_go_right", &console_go_right);
  lua_register(L, "console_go_left", &console_go_left);
  lua_register(L, "console_eval", &console_eval);

  luaL_openlibs(L);

  if(check_lua(L, luaL_dofile(L, "my_first.lua"))) {
    lua_getglobal(L, "Height");
    if(lua_isnumber(L, -1)) { Height = lua_tonumber(L, -1); lua_pop(L, -1); }
    lua_getglobal(L, "Width");
    if(lua_isnumber(L, -1)) { Width = lua_tonumber(L, -1); lua_pop(L, -1); }
    SDL_SetWindowSize(get_win(), Width, Height);

  }

  while(!should_quit) {
    // measure_scope();

    int editor_state;
    lua_getglobal(L, "editor_state");
    if(lua_isnumber(L, -1)) {
      editor_state = lua_tonumber(L, -1);
      lua_pop(L, -1);
    }

    int mode;
    lua_getglobal(L, "mode");
    if(lua_isnumber(L, -1)) {
      mode = lua_tonumber(L, -1);
      lua_pop(L, -1);
    }

    SDL_Event e;
    while(SDL_PollEvent(&e)) {
      switch(e.type) {
        case SDL_QUIT: {
          should_quit = true;
        } break;

        case SDL_KEYDOWN: {
          auto key = e.key.keysym.sym;
          auto mod = e.key.keysym.mod;

          if(mod & KMOD_CTRL && mod & KMOD_SHIFT) {
            interp_lua_table(L, "shift_ctrl", key);

          } else if(mod & KMOD_SHIFT) {
            interp_lua_table(L, "shift", key);

          } else if(mod & KMOD_CTRL) {
            interp_lua_table(L, "ctrl", key);

          } else { // no mod.
            interp_lua_table(L, "keys", key);
          }
        } break;

        case SDL_TEXTINPUT: {
          if(editor_state == Editor) {
            if(mode == InsertMode) { get_current_buffer()->put(e.text.text[0]); }

          } else if(editor_state == Console) {
              console_put(e.text.text[0]); 

          } else {
          }
        } break;

        case SDL_WINDOWEVENT: {
          if(e.window.event == SDL_WINDOWEVENT_RESIZED) {
            SDL_GetWindowSize(get_win(), &Width, &Height);
            auto tab = get_current_tab();
            tab->on_resize(Width, Height);
          }
        } break;

        case SDL_MOUSEWHEEL: {
          auto buffer = get_current_buffer();

          if(e.wheel.y > 0) {
            for(char i = 0; i < dt_scroll; i++) {
              buffer->scroll_up(mode == VisualMode);
            }

          } else if(e.wheel.y < 0) {
            for(char i = 0; i < dt_scroll; i++) {
              buffer->scroll_down(mode == VisualMode);
            }

          } else {
            assert(0);
          }
        } break;

        case SDL_MOUSEBUTTONDOWN: {
        } break;

        default: {
        } break;
      }
    }

    // update.
    switch(editor_state) {
      case Editor: {

        get_current_tab()->draw(mode == VisualMode);
        SDL_RenderPresent(get_renderer());
      } break;

      case Console: {
        draw_rect(0, get_console()->bottom_y, Width, font_height, WhiteColor);
        console_draw();
        SDL_RenderPresent(get_renderer());
      } break;
    }
  }


  lua_close(L);
    
  for(auto &pair: get_alphabet()) {
    SDL_DestroyTexture(pair.second);
  }
  SDL_DestroyRenderer(get_renderer());
  SDL_DestroyWindow(get_win());
  TTF_CloseFont(get_font());
  TTF_Quit();
  SDL_Quit();
  return 0;
}
