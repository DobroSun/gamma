#include "pch.h"
#include "console.h"
#include "font.h"
#include "interp.h"


static console_t console;
static bool is_input = false;


static void inc_cursor(unsigned *c, unsigned size) {
  if(*c == size) {
  } else {
    (*c)++;
  }
}

static void dec_cursor(unsigned *c, unsigned size) {
  if(*c == size) {
  } else {
    (*c)--;
  }
}

void console_init(int x) {
  console_on_resize(x);
}

void console_draw() {
  unsigned cursor = console.cursor;
  auto *buffer = &console.buffer;

  unsigned i = 0;
  for( ; i < buffer->size(); i++) {
    int px = i*font_width;
    int py = console.bottom_y;

    char c = (*buffer)[i];
    auto t = get_alphabet()[c];
    assert(t);

    copy_texture(t, px, py);

    if(i == cursor && is_input) {
      char c[2] = { (*buffer)[cursor], '\0'};
      if((*buffer)[cursor] == '\n') {
        c[0] = ' ';
      }
      draw_text_shaded(get_font(), (const char *)(&c), WhiteColor, BlackColor, px, py);
    }
  }

  if(cursor == buffer->size() && is_input) {
    int px = i*font_width;
    int py = console.bottom_y;

    char c[2] = {' ', '\0'};
    draw_text_shaded(get_font(), (const char *)(&c), WhiteColor, BlackColor, px, py);
  }
}

void console_clear() {
  console.buffer.clear();
  console.cursor = 0;
}

void console_put(char c) {
  is_input = true;
  console.buffer.add(c);

  if(console.cursor == console.buffer.size()) {
  } else {
    console.cursor++;
  }
}

void console_put_text(const char *t) {
  console_clear();
  for_each(t) {
    char c = *it;
    console_put(c);
  }
  is_input = false;
}

void console_backspace() {
  console.buffer.backspace();
  dec_cursor(&console.cursor, 0);
}

void console_run_command() {
  char f[MAX_CONSOLE_COMMAND_SIZE];
  console.buffer.to_string((char*)&f, MAX_CONSOLE_COMMAND_SIZE);
  interp((const char*)f);
  console_close();
}


void console_del() {
  console.buffer.del();
}

void console_go_left() {
  console.buffer.move_left();
  dec_cursor(&console.cursor, 0);
}


void console_go_right() {
  console.buffer.move_right();
  inc_cursor(&console.cursor, console.buffer.size());
}

void console_on_resize(int n_height) {
  console.bottom_y = (n_height > font_height)? n_height - font_height: 1;
}

void console_open() {
  mode = Console;
  console_clear();
  is_input = true;
}

void console_close() {
  mode = Editor;
  is_input = false;
}

console_t *get_console() {
  return &console;
}

