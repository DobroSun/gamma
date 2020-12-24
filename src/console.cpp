#include "pch.h"
#include "console.h"
#include "font.h"
#include "interp.h"


static console_t console;
static bool is_input = false;


void console_init() {
  console.buffer.chars.resize_with_no_init(console.buffer.gap_len);
}

void console_draw() {
  unsigned cursor = console.cursor;
  auto &buffer = console.buffer;
  if(!buffer.is_initialized()) { return; }

  unsigned i = 0;
  for( ; i < buffer.size(); i++) {
    int px = i*font_width;
    int py = console.bottom_y;

    char c = buffer[i];
    auto t = get_alphabet()[c];
    assert(t);

    copy_texture(t, px, py);

    if(i == cursor && is_input) {
      char c[] = { buffer[cursor], '\0'};
      if(buffer[cursor] == '\n') {
        c[0] = ' ';
      }
      draw_text_shaded(get_font(), (const char *)(&c), WhiteColor, BlackColor, px, py);
    }
  }

  if(cursor == buffer.size() && is_input) {
    int px = i*font_width;
    int py = console.bottom_y;

    char c[] = {' ', '\0'};
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
  if(console.cursor > 0) { console.cursor--; }
}

void console_run_command() {
  size_t size = console.buffer.size();
  char   cmd[size];

  for(size_t i = 0; i < size; i++) {
    cmd[i] = console.buffer[i];
  }
  cmd[size] = '\0';

  interp((const char*)cmd);
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
  console.bottom_y = (n_height > font_height)? n_height - font_height: 1;
}

console_t *get_console() {
  return &console;
}

