#ifndef GAMMA_CONSOLE_H
#define GAMMA_CONSOLE_H

struct console_t {
  gap_buffer buffer;
  unsigned cursor = 0;

  int bottom_y = 0;
};

void console_init(int);
void console_draw();
void console_clear();
void console_put(char);
void console_put_text(const char*);
void console_backspace();
void console_del();
void console_go_left();
void console_go_right();
void console_on_resize(int);
void console_open();
void console_close();
void console_run_command();
console_t *get_console();

#endif
