#include "pch.h"
#include "console.h"
#include "font.h"
#include "interp.h"


static console_t console;
static bool is_input = false;


void console_init() {
  console.buffer.chars.resize(console.buffer.gap_len);
}

void console_draw() {
  auto &buffer = console.buffer;
  if(!buffer.size()) { return; }

  const size_t cursor = console.cursor;
  const int    length = buffer.size();
  {
    char string[length + 1];
    string[length] = '\0';

    for(size_t i = 0; i < length; i++) {
      char c = buffer[i];
      string[i] = (c == '\n') ? ' ' : c;
    }

    draw_text_shaded(get_font(), string, console_color, console_text_color, 0, console.bottom_y);
  }
  if(is_input) {
    char c = (cursor >= length) ? ' ' : buffer[cursor];
    draw_text_shaded(get_font(), c, cursor_text_color, cursor_color, cursor*font_width, console.bottom_y);
  }
}

void console_clear() {
  console.buffer.clear();
  console.cursor = 0;
}

void console_put(char c) {
  is_input = true;
  console.buffer.add(c);

  if(console.buffer.size() == 1) {
    console.cursor = 1;
  } else {
    console.cursor++;
  }
}

void console_put_text(const char *t) {
  console_clear();
  while(*t != '\0') { console_put(*t++); }
  is_input = false;
}

void console_backspace() {
  console.buffer.backspace();
  if(console.cursor > 0) { console.cursor--; }
}

void console_run_command() {
  size_t size = console.buffer.size();
  char cmd[size+1];

  for(size_t i = 0; i < size; i++) {
    cmd[i] = console.buffer[i];
  }
  cmd[size] = '\0';

  interp_single_command(cmd);
}


void console_del() {
  console.buffer.del();
}

void console_go_left() {
  console.buffer.move_left();
  if(console.cursor > 0) { console.cursor--; }
}


void console_go_right() {
  console.buffer.move_right();

  if(console.cursor != console.buffer.size()) {
    console.cursor++;
  }
}

void console_on_resize(int n_height) {
  console.bottom_y = (n_height > font_height) ? (n_height-font_height) : 1;
}

console_t *get_console() {
  return &console;
}

