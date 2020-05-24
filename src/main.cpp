#include "gamma/pch.h"
#include "gamma/globals.h"
#include "gamma/utility.h"
#include "gamma/timer.h"
#include "gamma/cursor.h"
#include "gamma/view.h"


SDL_Renderer *renderer = nullptr;
TTF_Font *gfont = nullptr;
int tw = 0, th = 0; // Junks for texture quering.
Uint64 start = 0; // first line of used visible text in buffer.


int Init_SDL() {
  if(SDL_Init(SDL_INIT_EVENTS | SDL_INIT_TIMER)) {
    std::cerr << "Error Initializing SDL: " << SDL_GetError() << std::endl;
    return 1;
  }
  if(TTF_Init()) {
    std::cerr << "Error Initializing TTF: " << TTF_GetError() << std::endl;
    return 1;
  }
  return 0;
}

std::string read_args(int argc, char **argv) {
  return (argc < 2)? "": argv[1];
}



bool in_buffer(double x, double y) {
  return y >= TextUpperBound && x >= TextLeftBound && y < Height - TextBottomBound;
}





void LoadFile(String &buffer, std::vector<SDL_Texture *> &textures, std::fstream &file) {
  std::string input; unsigned count = 0;
  while(std::getline(file, input)) {
    buffer.push_back(input);

    // Prerender visible text.
    if(count < (unsigned)numrows()) {
      textures.push_back(load_courier(input, BlackColor));
      count++;
    }
  }
  textures.reserve(buffer.size());

  for( ; count < buffer.size(); count++) {
    textures[count] = load_courier(buffer[count], BlackColor);
  }
}


struct SelectedText {
  std::vector<int> i;
  std::vector<int> j;
};

void add_cursor(SelectedText &ss, const Cursor &c) {
  ss.i.push_back(c.i);
  ss.j.push_back(c.j);
}

void clear(SelectedText &ss) {
  ss.i.clear();
  ss.j.clear();
}



int main(int argc, char **argv) {
  if(Init_SDL()) return 1;
  auto filename =  read_args(argc, argv);

  std::fstream file{filename};
  if(!file) {
    std::cerr << "Error opening file: " << filename << std::endl;
    return 1;
  }



  SDL_Window *win = SDL_CreateWindow("Gamma",
                         SDL_WINDOWPOS_CENTERED, 
                         SDL_WINDOWPOS_CENTERED, 
                         Width, Height, 
                         SDL_WINDOW_RESIZABLE);
  renderer = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
  gfont = TTF_OpenFont((assets_fonts+courier).c_str(), ptsize);
  std::vector<SDL_Texture *> textures;
  String buffer;
  Cursor cursor{0, 0};

  SelectedText selected;

  int fw, fh;

  // Starts timer to update the cursor.
  SDL_TimerID cursor_timer = StartTimer(300);
  SDL_SetWindowMinimumSize(win, 300, 300); // Bug; sets only width == height.
  TTF_SizeText(gfont, "G", &fw, &fh);
  


  // Loading file in memory.
  // TODO: Load in multiple threads.
  LoadFile(buffer, textures, file);




  buffer_view b_view(buffer, 0, 0);
  SDL_Texture *cursor_texture = init_cursor(b_view, cursor);






  bool clicked = false;
  bool done = false;
  while(!done) {
    textures_view t_view(textures, start);

    SDL_Event e;
    if(SDL_PollEvent(&e)) {
      switch(e.type) {
        case SDL_QUIT: {
          done = true;
        } break;
          

        case SDL_MOUSEBUTTONDOWN: {
          auto &button = e.button;
          auto &b_type = button.button; 
          auto &b_click = button.clicks;



          if(b_type == SDL_BUTTON_LEFT && b_click == 3) {
              std::cout << "Triple click!" << std::endl;

          } else if(b_type == SDL_BUTTON_LEFT && b_click == 2) {
              std::cout << "Double click!" << std::endl;

          } else if(b_type == SDL_BUTTON_LEFT) {
            auto x = button.x; auto y = button.y;


            if(in_buffer(x, y)) {
              cursor = get_pos(x, y, fw);
              clicked = true;
              PauseTimer(); // FIXME: depends on current cursor_texture.
            }
            cursor = fix_cursor(b_view, cursor); // If cursor is out of buffer, it is set to the end of line+1.
          }
        } break;

        case SDL_MOUSEBUTTONUP: {
          auto &button = e.button;
          if(button.button == SDL_BUTTON_LEFT) {
            clicked = false;
            clear(selected);
            ResumeTimer();
          }
        } break;

        case SDL_MOUSEMOTION: {
          if(clicked) {
            auto &motion = e.motion;
            auto &x = motion.x; auto &y = motion.y;

            // For now handles only select in one line.
            Cursor cc;
            if(in_buffer(x, y)) {
              cc = get_pos(x, y, fw);
              if(cc != cursor) {
                add_cursor(selected, cc);
              }
            }
          }
        } break;
          

        case SDL_KEYDOWN: {
          if(e.key.keysym.sym == SDLK_ESCAPE) {
            done = true;
          }
        } break;
          

        case SDL_MOUSEWHEEL: {
        // FIXME: Bug: When scrolling down/up if cursor.i == scroll_speed; cursor is moving with window down/up.

          // Scrolling up/down.
          auto &wheel = e.wheel;
          if(wheel.y > 0) {
            start -= (start < scroll_speed)? start: scroll_speed;


            if(start == 0) break; // HACK: rewrite without this check. if start == 0 cursor still moves down.
            int diff = numrows() - cursor.i - 1;
            cursor.i += (diff < scroll_speed)? diff: scroll_speed;

          } else if(wheel.y < 0) {
            unsigned total = buffer.size()-numrows(); int ts = total-start;
            int speed = (ts < scroll_speed)? ts: scroll_speed;
            start += (start == total)? 0: speed;

            if(cursor.i != start) {
              cursor.i -= (cursor.i < scroll_speed)? cursor.i: scroll_speed;
            }
            // MighFail:
            // cursor.i finally becomes 0 and don't changes.
            // might fail with another scroll_speed.


          }
          cursor = fix_cursor(b_view, cursor); // If scrolls, cursor may take the position > size of line.
        } break;

        case SDL_WINDOWEVENT: {
          if(e.window.event == SDL_WINDOWEVENT_RESIZED) {
            SDL_GetWindowSize(win, &Width, &Height);
          }
        } break;
      }
    }
    b_view.start_i = start;
    cursor_texture = render_cursor(cursor_texture, b_view, cursor);

    // Background color.
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); 
    SDL_RenderClear(renderer);



    // Update window.
    for(int i = 0; i < numrows(); i++) {
      auto *txt = t_view[i];

      SDL_QueryTexture(txt, nullptr, nullptr, &tw, &th);
      SDL_Rect dst {TextLeftBound, TextUpperBound+i*fsize, tw, th};
      SDL_RenderCopy(renderer, txt, nullptr, &dst);

    }
    timer::update_cursor(cursor_texture, cursor, fw);



    // Update selected text.
    for(int i = 0; i < selected.i.size(); i++) {
      Cursor c {selected.i[i], selected.j[i]};
      auto new_txt = init_cursor(b_view, c);

      SDL_QueryTexture(new_txt, nullptr, nullptr, &tw, &th);
      SDL_Rect dst {TextLeftBound+fw*c.j, TextUpperBound+fsize*c.i, tw, th};
      SDL_RenderCopy(renderer, new_txt, nullptr, &dst);

      SDL_DestroyTexture(new_txt);
    }
    SDL_RenderPresent(renderer);
  }
  
  // TODO: Check whether I even need it or not.
  // Why freeing if programm ends.
  SDL_RemoveTimer(cursor_timer);
  SDL_DestroyTexture(cursor_texture);
  for(auto &txt: textures) {
    SDL_DestroyTexture(txt);
  }
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(win);
  TTF_CloseFont(gfont);
  TTF_Quit();
  SDL_Quit();
  return 0;
}
