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
  move_left,
  move_right,
  none,
};

enum direction_t : char {
  left,
  right,
  up,
  down,
};

struct selection_buffer_t {
  int start_index = -1;
  size_t size     = 0;
  moving_direction_t direction = none;
};

struct split_info_t {
  buffer_t *split_with = NULL;
  buffer_t *fake_split = NULL;
  split_type_t type;
};

struct file_buffer_t {
  gap_buffer buffer;
  array<buffer_t *> undo;
  bool is_used = false;
};



struct buffer_t {
  file_buffer_t *file;
  bool is_used = false;
  string_t filename;

  split_info_t split;

  // Position on the window.
  int start_x, start_y, width, height;
  unsigned cursor = 0, n_character = 0, n_line = 0, total_lines = 0;
  unsigned offset_on_line = 0, offset_from_beginning = 0, start_pos = 0, saved_pos = 0;

  buffer_t() = default;
  buffer_t(const buffer_t &) = delete;
  buffer_t &operator=(const buffer_t &) = delete;
  buffer_t(buffer_t &&) = delete;
  buffer_t &operator=(buffer_t &&) = delete;


  void init(int,int,int,int);
  void draw(bool) const;
  void save();
  void on_resize(int,int,int,int);

  // @CleanUp all functions.
  void scroll_down();
  void scroll_up();
  void go_right();
  void go_left();
  void put_backspace();
  void put_return();
  void put_delete();
  void put_tab();
  void put(char);

  // @RemoveME:
  int compute_go_down();
  int compute_go_up();


  void draw_cursor(char, int, int, SDL_Color, SDL_Color) const;
  void draw_line(int, int, bool) const;

  void shift_beginning_down();
  void shift_beginning_up();

  int count_total_lines() const;
  int get_line_length(int) const;
  int get_cursor_pos_on_line() const;

  int get_relative_pos_x(int) const;
  int get_relative_pos_y(int) const;
};


struct tab_t {
  array<buffer_t *> buffers;
  bool is_used = false;

  void on_resize(int,int);
  void draw(bool) const;
};

int number_lines_fits_in_window(const buffer_t *);
int number_chars_on_line_fits_in_window(const buffer_t *);

tab_t *get_current_tab();
buffer_t *get_current_buffer();
selection_buffer_t &get_selection();

void init(int, char**);
void update();

void open_new_buffer(string_t &);
void open_existing_or_new_buffer(const literal &);
void open_existing_buffer(buffer_t *);

selection_buffer_t *get_selection_buffer();
void delete_selected();
void copy_selected();
void clear_selection();

void paste_from_global_copy();

void go_to_line(int);
void save();

void close_buffer(buffer_t *);
void change_buffer(buffer_t *, direction_t);
#define change_buffer_to_left(b)  change_buffer(b, left)
#define change_buffer_to_right(b) change_buffer(b, right)
#define change_buffer_to_up(b)    change_buffer(b, up)
#define change_buffer_to_down(b)  change_buffer(b, down)

void do_split(buffer_t *, buffer_t *, split_type_t);

int go_word_forward();
int go_word_backwards();

int compute_to_beginning_of_line();
int compute_to_end_of_line();

void undo(buffer_t *);
void save_current_state_for_undo(buffer_t *);






inline void copy_window_position(buffer_t *a, const buffer_t *b) {
  a->is_used = b->is_used;
  a->start_x = b->start_x;
  a->start_y = b->start_y;
  a->width   = b->width;
  a->height  = b->height;
  a->cursor = b->cursor;
  a->n_character = b->n_character;
  a->n_line = b->n_line;
  a->total_lines = b->total_lines;
  a->offset_on_line = b->offset_on_line;
  a->offset_from_beginning = b->offset_from_beginning;
  a->start_pos = b->start_pos;
  a->saved_pos = b->saved_pos;
}

#endif
