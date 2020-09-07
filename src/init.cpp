#include "gamma/pch.h"
#include "gamma/init.h"


static SDL_Window *win = nullptr;
static SDL_Renderer *renderer = nullptr;
static int fw = 0, fh = 0;

static EditorMode mode = EditorMode::Editor;


bool Init_SDL() {
  bool failed = false;

  defer { if(failed) SDL_Quit(); };
  if(SDL_Init(SDL_INIT_EVENTS | SDL_INIT_TIMER)) {
    fprintf(stderr, "%s: %s", "Error Initializing SDL.\n", SDL_GetError());
    failed = true;
    return failed;
  }

  defer { if(failed) TTF_Quit(); };
  if(TTF_Init()) {
    fprintf(stderr, "%s: %s", "Error Initializing TTF.\n", TTF_GetError());
    failed = true;
    return failed;
  }


  win = SDL_CreateWindow("Gamma",
                         SDL_WINDOWPOS_CENTERED,
                         SDL_WINDOWPOS_CENTERED,
                         Width, Height,
                         SDL_WINDOW_RESIZABLE);
  defer { if(failed) SDL_DestroyWindow(win); };
  if(!win) {
    fprintf(stderr, "%s: %s", "Error Creating SDL_Window.\n", SDL_GetError());
    failed = true;
    return failed;
  }

  SDL_SetWindowMinimumSize(win, 300, 300); // @Bug: sets only width == height.


  renderer = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
  defer { if(failed) SDL_DestroyRenderer(renderer); };
  if(!renderer) {
    fprintf(stderr, "%s: %s", "Error Creating SDL_Renderer.\n", SDL_GetError());
    failed = true;
    return failed;
  }
  return failed;
}


SDL_Window *get_win() {
  return win;
}

SDL_Renderer *get_renderer() {
  return renderer;
}

EditorMode get_editor_mode() {
  return mode;
}
