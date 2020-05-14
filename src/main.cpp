#include "gamma/pch.h"
#include "gamma/globals.h"
#include "gamma/utility.h"
#include "gamma/load_surf.h"


std::string filename;
SDL_Window *win = nullptr;
SDL_Renderer *renderer = nullptr;


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

  // Bug; sets only width == height.
  SDL_SetWindowMinimumSize(win, 300, 300);



  // Loading file in memory.
  std::string input, acc;
  std::vector<SDL_Texture *> textures;
  while(std::getline(file, input)) {
    acc += input + "\n";

    textures.push_back(load_text(input+"\n", "Quicksand-Regular.ttf", ptsize, WhileColor));
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
              start -= (start < 2)? 0: 2;

            } else if(e.wheel.y < 0) {
              start += ((unsigned)start > textures.size()-totalrows()-6)? 0: 2;
              // Why -6? It just works!

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


      int texW = 0, texH = 0;
      // Copying all textures even those that aren't on window.
      for(int i = start, j = 0; (unsigned)i < textures.size(); i++, j++) {
        SDL_QueryTexture(textures[i], nullptr, nullptr, &texW, &texH);
        SDL_Rect dst {TextLeftBound, TextUpperBound+j*ptsize + blines, texW, texH};
        SDL_RenderCopy(renderer, textures[i], nullptr, &dst);
      }

      SDL_RenderPresent(renderer);
  };




  for(auto &txt: textures) {
    SDL_DestroyTexture(txt);
  }
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(win);
  TTF_Quit();
  SDL_Quit();
  return 0;
}
