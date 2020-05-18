#include "gamma/pch.h"
#include "gamma/globals.h"
#include "gamma/utility.h"
#include "gamma/cache.h"


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
struct Cursor {
  int x, y;
};

bool in_buffer(const String &buffer, const Cursor &cursor) {
  return cursor.x >= 0 && cursor.y >= 0 &&
         cursor.x < buffer.size() && cursor.y < buffer[0].size();
}
*/

/*
Cursor actual_cursor(const String &buffer, const Cursor &cursor) {
}
*/



















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
  TextureCache textures; textures.set_maxsize(numrows());
  String buffer;
  //Cursor cursor{0, 0};

  // Bug; sets only width == height.
  SDL_SetWindowMinimumSize(win, 300, 300);





  // Loading file in memory.
  std::string input; int count = 0;
  while(std::getline(file, input)) {
    const std::string ii = input+"\n";
    buffer.push_back(ii);

    // Prerendering text visible on window.
    if(count < textures.maxsize) {
      textures.push_right(load_normal(ii));
      count++;
    }
  }
  // All visible textures are cached.
  // and dynamicly updated on user actions.
  // So on window resizing event, size of vector<textures> should
  // change, but rerendering on resizing is too slow.
  // The idea I should keep `maxsize` textures ready to be showed
  // on window resize action.

  // But that way it can't show bottom of window, which is
  // hidden `maxsize - numrows()` lines of text.

  // What should I do?
  // Uh?




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
              update_on_scroll<false>(textures, buffer, start);
              start -= (start < scroll_speed)? start: scroll_speed;


            } else if(e.wheel.y < 0) {
              update_on_scroll<true>(textures, buffer, start);

              int total = buffer.size()-numrows();
              int speed = (total-start < scroll_speed)? total-start: scroll_speed;
              start += (start == total)? 0: speed;
              std::cout << start << std::endl;

            } else {
            }
            break;

          case SDL_WINDOWEVENT:
            if(e.window.event == SDL_WINDOWEVENT_RESIZED) {
              SDL_GetWindowSize(win, &Width, &Height);

              //textures.set_maxsize(numrows());
            }
            break;
        }
      }

      // Background color.
      SDL_SetRenderDrawColor(renderer, 69, 6, 12, 255); 
      SDL_RenderClear(renderer);




      // Update window.
      int texW = 0, texH = 0;
      for(int j = 0; j < textures.maxsize; j++) {
        auto txt = textures.cached[j];

        SDL_QueryTexture(txt, nullptr, nullptr, &texW, &texH);
        SDL_Rect dst {TextLeftBound, TextUpperBound+j*(ptsize + blines), texW, texH};
        SDL_RenderCopy(renderer, txt, nullptr, &dst);
      }
      SDL_RenderPresent(renderer);
  }
  

  for(auto &txt: textures.cached) {
    SDL_DestroyTexture(txt);
  }
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(win);
  TTF_Quit();
  SDL_Quit();
  return 0;
}
