#include "gamma/pch.h"
#include "gamma/init.h"
#include "gamma/buffer.h"
#include "gamma/font.h"
#include "gamma/commands.h"
#include "gamma/console.h"



int main(int argc, char **argv) {
  if(Init_SDL()) return 1;

  init(argc, argv);
  defer { \
    for(auto &pair: get_alphabet()) { \
      SDL_DestroyTexture(pair.second); \
    } \
    SDL_DestroyRenderer(get_renderer()); \
    SDL_DestroyWindow(get_win()); \
    TTF_CloseFont(get_font()); \
    TTF_Quit(); \
    SDL_Quit(); \
  };
    

  bool selection_mode = false;
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


    SDL_Event e;
    while(SDL_PollEvent(&e)) {

      switch(e.type) {
        case SDL_QUIT: {
          should_quit = true;
        } break;

        case SDL_KEYDOWN: {
          switch(mode) {
            case Editor: {
              auto key = e.key.keysym.sym;
              auto mod = e.key.keysym.mod;

              if(mod & KMOD_CTRL && mod & KMOD_SHIFT) {

              } else if(mod & KMOD_SHIFT) {

              } else if(mod & KMOD_CTRL) {
                if(key == SDLK_r) {
                  console_open();

                } else if(key == SDLK_s) {
                  save();

                } else if(key == SDLK_c) {
                  assert(mode == Editor);
                  selection_mode = true;

                  auto selected = get_selection_buffer();
                  auto buffer   = get_current_buffer(); 

                  selected->start_index = buffer->cursor;
                  selected->start_line  = buffer->n_line;
                  selected->start_char  = buffer->n_character;
                  selected->size        = 0;
                  selected->direction   = none;
                }

              } else { // no mod.
                if(key == SDLK_ESCAPE) {
                  // @Temporary: 
                  if(selection_mode) {
                    selection_mode = false;
                  } else {
                    should_quit = true;
                  }

                } else if(key == SDLK_RETURN) {
                  get_current_buffer()->put_return();
                
                } else if(key == SDLK_BACKSPACE) {
                  get_current_buffer()->put_backspace();
                
                } else if(key == SDLK_DELETE) {
                  get_current_buffer()->put_delete();
                
                } else if(key == SDLK_LEFT) {
                  get_current_buffer()->go_left(selection_mode);
                
                } else if(key == SDLK_RIGHT) {
                  get_current_buffer()->go_right(selection_mode);
                
                } else if(key == SDLK_DOWN) {
                  get_current_buffer()->go_down(selection_mode);
                
                } else if(key == SDLK_UP) {
                  get_current_buffer()->go_up(selection_mode);

                } else if(key == SDLK_d) {
                  delete_selected();

                } else if(key == SDLK_y) {
                  copy_selected();
                
                } else if(key == SDLK_p) {
                  // @Incomplete:
                  // It's not using selection_mode so while SDL_TEXTINPUT gets triggered
                  // it puts `p`.
                  paste_from_global_copy(); 

                } else {
                }

              }
            } break;
            // Editor.

            case Console: {
              auto key = e.key.keysym.sym;
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
            } break;
            // Console.
          }
        } break;
        // SDL_KEYDOWN.

        case SDL_TEXTINPUT: {
          switch(mode) {
            case Editor: {
              if(selection_mode) {
                clear_selection();
                selection_mode = false;

              } else {
                char c = e.text.text[0];
                get_current_buffer()->put(c);

              }
            } break;

            case Console: {
              char c = e.text.text[0];
              console_put(c);
            } break;
          }
        } break;
        // SDL_TEXTINPUT.

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
          if(e.wheel.y > 0) {
            for(char i = 0; i < dt_scroll; i++) {
              buffer->scroll_up(selection_mode);
            }

          } else if(e.wheel.y < 0) {
            for(char i = 0; i < dt_scroll; i++) {
              buffer->scroll_down(selection_mode);
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
    switch(mode) {
      case Editor: {
        auto renderer = get_renderer();
        SDL_SetRenderDrawColor(renderer, WhiteColor.r, WhiteColor.g, WhiteColor.b, WhiteColor.a); 
        SDL_RenderClear(renderer);

        get_current_tab()->draw(selection_mode);
        SDL_RenderPresent(renderer);
      } break;

      case Console: {
        draw_rect(0, get_console()->bottom_y, Width, font_height, WhiteColor);
        console_draw();
        SDL_RenderPresent(get_renderer());
      } break;
    }
  }
  return 0;
}
