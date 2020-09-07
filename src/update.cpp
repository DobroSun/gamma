#include "gamma/pch.h"
#include "gamma/update.h"
#include "gamma/init.h"
#include "gamma/buffer.h"
#include "gamma/input.h"

#if 0
static int tw = 0, th = 0;
static void copy_character_on_screen(SDL_Renderer *renderer, SDL_Texture *t, int x_pixels, int y_pixels) {
  SDL_QueryTexture(t, nullptr, nullptr, &tw, &th);
  SDL_Rect dst {x_pixels, y_pixels, tw, th};
  SDL_RenderCopy(renderer, t, nullptr, &dst);
}

#endif

#if 0
static void update_editor(SDL_Renderer *renderer, texture_map &alphabet, texture_map &selected) {
  // Set background color.
  SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); 
  SDL_RenderClear(renderer);

  const auto &b_view = get_buffer();
  auto &cursor = b_view.cursor;
  const unsigned offset = b_view.start_j;
  const unsigned max_size = buffer_width() / fw;

  for(int line = b_view.start, i = 0; i < numrows(); i++, line++) {
    auto string = b_view.get_view(line);

    for(unsigned j = offset*max_size; j < string.size(); j++) {
      char c = string[j];
      auto char_texture = alphabet[c];
      assert(char_texture);

      int x_pixels = TextLeftBound  + (j - offset*max_size) * fw ;
      int y_pixels = TextUpperBound + i * fsize;

      // Update character.
      copy_character_on_screen(renderer, char_texture, x_pixels, y_pixels);


      // Update selected symbol.
      if(line == cursor.i && (int)j == cursor.j) {
        auto cursor_texture = selected[c];
        copy_character_on_screen(renderer, cursor_texture, x_pixels, y_pixels);
      }
    }
  }
}

static void update_console(SDL_Renderer *renderer, texture_map &alphabet, texture_map &selected) {
  // Nothing for now.
  SDL_SetRenderDrawColor(renderer, 200, 200, 200, 0); 
  SDL_RenderClear(renderer);

  const int y_pixels = TextUpperBound + (numrows()+1) * fsize;

  auto &console = get_buffer().console;
  for(unsigned i = 0; i < console.size()-1; i++) {
    char c = console[i];

    auto char_texture = alphabet[c];
    assert(char_texture);

    int x_pixels = TextLeftBound + i * fw;
    copy_character_on_screen(renderer, char_texture, x_pixels, y_pixels);
  }

  //(void)renderer;
  //(void)alphabet;
  (void)selected;
}
#endif
void update(texture_map &alphabet, texture_map &selected) {
  
#if 0
  switch(get_editor_mode()) {
    case Editor: {
      update_editor(renderer, alphabet, selected);
    } break;

    case Console: {
      update_console(renderer, alphabet, selected);
    } break;
  }
#endif
}
