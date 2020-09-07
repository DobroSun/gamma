#ifndef GAMMA_INIT_H
#define GAMMA_INIT_H

enum class EditorMode {
  Editor,
  Console,
};

bool Init_SDL();

SDL_Window *get_win();
SDL_Renderer *get_renderer();
EditorMode get_editor_mode();
#endif
