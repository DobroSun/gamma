#ifndef GAMMA_BUFFER_H
#define GAMMA_BUFFER_H

enum split_type_t : char {
  hsp_type,
  vsp_type,
};

enum editing_mode_t : char {
  normal_m,
  insert_m,
  select_m,
};

enum moving_direction_t : char {
  left,
  right,
  none,
};

struct selection_buffer_t {
  int start_index = -1, start_line, start_char;
  size_t size = 0;
  moving_direction_t direction = none;
};

struct file_buffer_t {
  gap_buffer buffer;
  bool is_used = false;
};

struct buffer_t {
  file_buffer_t *file;
  bool is_used = false;
  string_t filename;


  // Position on the window.
  int start_x = 0, start_y = 0, width = Width, height = Height;
  unsigned cursor = 0, n_character = 0, n_line = 0;
  unsigned offset_on_line = 0, offset_from_beginning = 0, start_pos = 0;


  void init(int,int,int,int);
  void draw(bool) const;
  void save();
  void on_resize(int,int,int,int);

  // @CleanUp all functions.
  void scroll_down(bool selecting=false);
  void scroll_up(bool selecting=false);
  void go_down(bool selecting=false);
  void go_up(bool selecting=false);
  void go_right(bool selecting=false);
  void go_left(bool selecting=false);
  void put_backspace();
  void put_return();
  void put_delete();
  void put_tab();
  void put(char);

  void move_to(size_t);

  void draw_cursor(char, int, int, SDL_Color, SDL_Color) const;
  void draw_line(int, int, bool) const;

  void shift_beginning_down();
  void shift_beginning_up();

  int get_total_lines() const;
  int get_line_length(int) const;
  int get_cursor_pos_on_line() const;

  int get_relative_pos_x(int) const;
  int get_relative_pos_y(int) const;

  void inc_cursor();
};

struct tab_t {
  buffer_t buffers[4];
  bool is_used = false;

  void on_resize(int,int);
  void draw(bool) const;
};

int number_lines_fits_in_window(const buffer_t *);
int number_chars_on_line_fits_in_window(const buffer_t *);

tab_t *get_current_tab();
buffer_t *get_current_buffer();

void init(int, char**);
void update();

void open_new_buffer(literal);
void open_existing_or_new_buffer(literal);
void open_existing_buffer(buffer_t *);

selection_buffer_t *get_selection_buffer();
void delete_selected();
void copy_selected();
void clear_selection();

void paste_from_global_copy();

void go_to_line(int);
void save();
void quit(int);

void do_split(const literal &l, split_type_t);
#define hsplit(l) do_split(l, hsp_type)
#define vsplit(l) do_split(l, vsp_type)

void cursor_right();
void cursor_left();
void cursor_up();
void cursor_down();

#endif
