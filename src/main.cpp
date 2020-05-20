#include "gamma/pch.h"
#include "gamma/globals.h"
#include "gamma/utility.h"
#include "gamma/timer.h"



SDL_Window *win = nullptr;
SDL_Renderer *renderer = nullptr;
TTF_Font *gfont = nullptr;


std::string filename;
Uint64 start = 0; // line from which updating text starts.
int tw = 0, th = 0; // used in texture quering.


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

void read_args(int argc, char **argv) {
  filename = (argc < 2)? "": argv[1];
}



struct Cursor {
  int i, j;
};

bool in_buffer(const String &buffer, const Cursor &cursor) {
  return cursor.i >= 0 && cursor.j >= 0 &&
         cursor.i < buffer.size() && cursor.j < buffer[0].size();
}
bool in_buffer(double x, double y) {
  return y >= TextUpperBound && x >= TextLeftBound;
}

Cursor get_pos(double x, double y) {
  Cursor ret;

  // Start of text buffer.
  auto xx = x - TextLeftBound;
  auto yy = y - TextUpperBound;

  ret.i = xx;
  ret.j = yy + start;
  return ret;
}





std::string str(char k) {
  return {k};
}
std::string str(std::string &k) {
  return k;
}
std::string str(const char *k) {
  return {k};
}

SDL_Texture *get_cursored(const String buffer, const Cursor &c) {
  SDL_Surface *t = TTF_RenderText_Shaded(gfont, str(buffer[c.i][c.j]).c_str(), WhiteColor, BlackColor);
  SDL_Texture *cc = SDL_CreateTextureFromSurface(renderer, t);
  SDL_FreeSurface(t);
  return cc;
}




int main(int argc, char **argv) {
  if(Init_SDL()) return 1;
  read_args(argc, argv);

  std::fstream file{filename};
  if(!file) {
    std::cerr << "Error opening file: " << filename << std::endl;
    return 1;
  }




  win = SDL_CreateWindow("Gamma",
                         SDL_WINDOWPOS_CENTERED, 
                         SDL_WINDOWPOS_CENTERED, 
                         Width, Height, 
                         SDL_WINDOW_RESIZABLE);
  renderer = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
  gfont = TTF_OpenFont((assets_fonts+courier).c_str(), ptsize);
  int fw, fh;
  std::vector<SDL_Texture *> textures;
  String buffer;
  Cursor cursor{0, 0};


  // Starts timer to update the cursor.
  StartTimer();
  


  // Bug; sets only width == height.
  SDL_SetWindowMinimumSize(win, 300, 300);
  TTF_SizeText(gfont, "G", &fw, &fh);




  // Loading file in memory.
  std::string input; int count = 0;
  while(std::getline(file, input)) {
    buffer.push_back(input);

    // Prerender visible text.
    if(count < numrows()) {
      textures.push_back(load_courier(input, BlackColor));
      count++;
    }
  }
  textures.reserve(buffer.size());

  // Have to do this in other thread.
  for( ; count < buffer.size(); count++) {
    textures[count] = load_courier(buffer[count], BlackColor);
  }



  SDL_Texture *cursor_txt = get_cursored(buffer, cursor);





  bool done = false;
  while(!done) {
      SDL_Event e;
      if(SDL_PollEvent(&e)) {
        switch(e.type) {
          case SDL_QUIT:
            done = true;
            break;


          case SDL_MOUSEBUTTONDOWN: {
            auto &button = e.button;
            if(button.button == SDL_BUTTON_LEFT) {
              auto x = button.x; auto y = button.y;
              if(in_buffer(x, y)) {
                cursor = std::move(get_pos(x, y)); // FIXME: Not sure whether it is copy or move.
              }
            }
            break;
          }
  
          case SDL_KEYDOWN:
            if(e.key.keysym.sym == SDLK_ESCAPE) {
              done = true;
            }
						break;

          case SDL_MOUSEWHEEL: {
            auto &wheel = e.wheel;
            if(wheel.y > 0) {
              start -= (start < scroll_speed)? start: scroll_speed;


            } else if(wheel.y < 0) {
              int total = buffer.size()-numrows(); int ts = total-start;
              int speed = (ts < scroll_speed)? ts: scroll_speed;
              start += (start == total)? 0: speed;

            } else {
            }
            break;
          }

          case SDL_WINDOWEVENT:
            if(e.window.event == SDL_WINDOWEVENT_RESIZED) {
              SDL_GetWindowSize(win, &Width, &Height);
            }
            break;
        }
      }

      // Background color.
      SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); 
      SDL_RenderClear(renderer);


      // Update window.
      for(int i = start, j = 0; j < numrows(); i++, j++) {
        auto *txt = textures[i];

        SDL_QueryTexture(txt, nullptr, nullptr, &tw, &th);
        SDL_Rect dst {TextLeftBound, TextUpperBound+j*fsize, tw, th};
        SDL_RenderCopy(renderer, txt, nullptr, &dst);

      }
      timer::update_cursor(cursor_txt, cursor, fw);



      SDL_RenderPresent(renderer);
  }
  

  SDL_DestroyTexture(cursor_txt);
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
