#include "gamma/pch.h"
#include "gamma/globals.h"
#include "gamma/utility.h"


std::string filename;
SDL_Window *win = nullptr;
SDL_Renderer *renderer = nullptr;

using String = std::vector<std::string>;


int Init_SDL() {
  if(SDL_Init(SDL_INIT_EVENTS)) {
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


/*
enum threebool {
  yes,
  no,
  none,
};
*/

struct TextureCache {
  std::vector<SDL_Texture *> textures;
};



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
  TextureCache cache;
  String buffer;

  // Bug; sets only width == height.
  SDL_SetWindowMinimumSize(win, 300, 300);



  // Loading file in memory.
  std::string input;
  while(std::getline(file, input)) {
    buffer.push_back(input+"\n");
  }



  int start = 0; // for scrolling text.
  bool done = false;
  while(!done) {

      SDL_Event e;
      if(SDL_PollEvent(&e)) {
        switch(e.type) {
          case SDL_QUIT:
            done = true;
            break;
  
          case SDL_KEYDOWN:
            if(e.key.keysym.sym == SDLK_ESCAPE) {
              done = true;
            }
						break;

          case SDL_MOUSEWHEEL:
            if(e.wheel.y > 0) {
              start -= (start < scroll_speed)? 0: scroll_speed;

            } else if(e.wheel.y < 0) {
              int total = buffer.size()-numrows();
              int speed = (total-start < scroll_speed)? total-start: scroll_speed;

              assert(start <= total);
              start += (start == total)? 0: speed;

            }
            break;

          case SDL_WINDOWEVENT:
            if(e.window.event == SDL_WINDOWEVENT_RESIZED) {
              SDL_GetWindowSize(win, &Width, &Height);
            }
            break;
        }
      }

      // Background color.
      SDL_SetRenderDrawColor(renderer, 69, 6, 12, 255); 
      SDL_RenderClear(renderer);


      // On each iteration we are rerendering every texture,
      // even though some of them were used in previous loop.
      // TODO: Cache used ones.


      int texW = 0, texH = 0;
      assert((unsigned)start+numrows() <= buffer.size());
      for(int i = start, j = 0; i < start+numrows(); i++, j++) {
        assert(j < numrows());


        // Rendering text.
        auto &line = buffer[i];
        auto txt = load_text(line, orig_font, ptsize, WhileColor);

        SDL_QueryTexture(txt, nullptr, nullptr, &texW, &texH);
        SDL_Rect dst {TextLeftBound, TextUpperBound+j*(ptsize + blines), texW, texH};
        SDL_RenderCopy(renderer, txt, nullptr, &dst);

        // Destroying texture.
        SDL_DestroyTexture(txt);
      }

      SDL_RenderPresent(renderer);
  };
  

  for(auto &txt: cache.textures) {
    SDL_DestroyTexture(txt);
  }
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(win);
  TTF_Quit();
  SDL_Quit();
  return 0;
}
