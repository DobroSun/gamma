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
  lua_register(L, "go_down",  &go_down);
  lua_register(L, "go_up",    &go_up);
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

  luaL_openlibs(L);

  if(check_lua(L, luaL_dofile(L, "my_first.lua"))) {
    lua_getglobal(L, "Height");
    if(lua_isnumber(L, -1)) { Height = lua_tonumber(L, -1); lua_pop(L, -1); }
    lua_getglobal(L, "Width");
    if(lua_isnumber(L, -1)) { Width = lua_tonumber(L, -1); lua_pop(L, -1); }
    SDL_SetWindowSize(get_win(), Width, Height);

  }

  while(!should_quit) {
    #if 0
    auto begin = std::chrono::steady_clock::now();
    defer {
      auto end = std::chrono::steady_clock::now();
      double cur_time = std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count();
      unsigned to_sleep = 1000000/60 - cur_time;
      //sleep(to_sleep);
      if(show_fps) {
        end = std::chrono::steady_clock::now();
        cur_time = std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count();
        char fps[64];
        sprintf(fps, "******* FPS is: %lf *******", 1000000 / cur_time);
        print(fps);
      }
    };
    #endif

    int editor_state;
    lua_getglobal(L, "editor_state");
    if(lua_isnumber(L, -1)) {
      editor_state = lua_tonumber(L, -1);
      lua_pop(L, -1);
    }

    SDL_Event e;
    while(SDL_PollEvent(&e)) {
      switch(e.type) {
        case SDL_QUIT: {
          should_quit = true;
        } break;

        case SDL_KEYDOWN: {
          switch(editor_state) {
            case Editor: {
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
            // Editor.

            case Console: {
            #if 0
              if(mod & KMOD_CTRL && mod & KMOD_SHIFT) {

              } else if(mod & KMOD_CTRL) {

              } else if(mod & KMOD_SHIFT) {
                interp_lua_table(L, "ctrl", key);

              } else { // no mod.
                interp_lua_table(L, "ctrl", key);
              }
              #endif
              #if 0
              if(key == SDLK_ESCAPE) {
                console_close();

              } else if(key == SDLK_RETURN) {
                console_run_command();
              
              } else if(key == SDLK_BACKSPACE) {
                console_backspace();
              
              } else if(key == SDLK_DELETE) {
                console_del();
              
              } else if(key == SDLK_LEFT) {
                console_go_left();
              
              } else if(key == SDLK_RIGHT) {
                console_go_right();
              }
              #endif
            } break;
            // Console.
          }
        } break;
        // SDL_KEYDOWN.

        case SDL_WINDOWEVENT: {
          if(e.window.event == SDL_WINDOWEVENT_RESIZED) {
            SDL_GetWindowSize(get_win(), &Width, &Height);
            auto tab = get_current_tab();
            tab->on_resize(Width, Height);
          }
        } break;
        // SDL_WINDOWEVENT.

        case SDL_MOUSEWHEEL: {
          auto buffer = get_current_buffer();
          bool with_selection;
          lua_getglobal(L, "mode");
          if(lua_isnumber(L, -1)) { 
            with_selection = (lua_tonumber(L, -1) == VisualMode);
            lua_pop(L, -1);
          }

          if(e.wheel.y > 0) {
            for(char i = 0; i < dt_scroll; i++) {
              buffer->scroll_up(with_selection);
            }

          } else if(e.wheel.y < 0) {
            for(char i = 0; i < dt_scroll; i++) {
              buffer->scroll_down(with_selection);
            }

          } else {
            assert(0);
          }
        } break;
        // SDL_MOUSEWHEEL.

        case SDL_MOUSEBUTTONDOWN: {
        } break;
        // SDL_MOUSEBUTTONDOWN.

        default: {
        } break;
      }
    }

    // update.
    switch(editor_state) {
      case Editor: {
        bool with_selection;
        lua_getglobal(L, "mode");
        if(lua_isnumber(L, -1)) {
          with_selection = (lua_tonumber(L, -1) == VisualMode);
          lua_pop(L, -1);
        }

        get_current_tab()->draw(with_selection);
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
