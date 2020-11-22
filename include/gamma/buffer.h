#ifndef GAMMA_BUFFER_H
#define GAMMA_BUFFER_H


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
  void draw() const;
  void save();
  void on_resize(int,int,int,int);

  // @CleanUp all functions.
  void scroll_down();
  void scroll_up();
  void go_down();
  void go_up();
  void go_right();
  void move_left();
  void go_left();
  void put_backspace();
  void put_return();
  void put_delete();
  void put(char);

  int get_total_lines() const;

private:
  void draw_cursor(char, int, int, SDL_Color, SDL_Color) const;
  void draw_line(int, int) const;

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

  void act_on_non_text_character(int&, int&, char) const;
};


struct tab_t {
  buffer_t buffers[4];
  bool is_used = false;

  void on_resize(int,int);
  void draw() const;
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
#endif
