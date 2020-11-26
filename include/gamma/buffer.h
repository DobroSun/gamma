#ifndef GAMMA_BUFFER_H
#define GAMMA_BUFFER_H

enum moving_direction_t: char {
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
  void move_left();
  void go_left(bool selecting=false);
  void put_backspace();
  void put_return();
  void put_delete();
  void put(char);

  int get_total_lines() const;

private:
  void draw_cursor(char, int, int, SDL_Color, SDL_Color) const;
  void draw_line(int, int, bool) const;

  int  get_line_length(int) const;

  bool is_last_line() const;
  bool is_first_line() const;
  bool cursor_on_last_line() const;
  bool cursor_on_first_line() const;
  bool char_fits_on_buffer_width() const;

  int num_chars_fits_to_buffer_width() const;
  int num_to_shift_down_on_scrolling() const;
  int num_to_shift_up_on_scrolling() const;

  int get_relative_pos_x(int) const;
  int get_relative_pos_y(int) const;

  int get_cursor_pos_x() const;

  void inc_cursor();
  void dec_cursor();

  void inc_start(int);
  void dec_start(int);
};


struct tab_t {
  buffer_t buffers[4];
  bool is_used = false;

  void on_resize(int,int);
  void draw(bool) const;
};

tab_t *get_current_tab();
buffer_t *get_current_buffer();

void init(int, char**);
void update();

void open_new_buffer(literal);
void open_existing_or_new_buffer(literal);
void open_existing_buffer(buffer_t *);


FILE *get_file_or_create(const char *, const char *);
void read_entire_file(gap_buffer *, FILE *);

selection_buffer_t *get_selection_buffer();
void delete_selected();
void clear_selection();
#endif
