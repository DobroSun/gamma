#ifndef GAMMA_BUFFER_H
#define GAMMA_BUFFER_H

enum split_type_t : u8 {
  hsp_type,
  vsp_type,
};

enum direction_t : u8 {
  left = 0,
  right,
  up,
  down,
  none,
};

struct selection_buffer_t {
  int start_index = -1;
  size_t size     = 0;
  direction_t direction = none;
};


struct file_buffer_t {
  gap_buffer buffer;
  array<buffer_t> undo;
  array<buffer_t> redo;
};

struct Loc { size_t index, l, c, size; };

struct buffer_t {
  file_buffer_t *file = NULL;
  bool is_used = false;
  string filename;

  array<Loc> found;

  int start_x, start_y, width, height;
  size_t cursor = 0, n_character = 0, n_line = 0, total_lines = 0;
  size_t offset_on_line = 0, offset_from_beginning = 0, start_pos = 0, saved_pos = 0;





  void init(int,int,int,int);
  void draw(bool) const;
  void save();

  // @CleanUp all functions.
  void scroll_down();
  void scroll_up();
  void go_right();
  void go_left();
  void put_backspace();
  void put_return();
  void put_delete();
  void put(char);


  // @Temporary: @RemoveMe:
  void go_down();
  void go_up();

  int compute_go_down();
  int compute_go_up();


  void draw_cursor(char, int, int, SDL_Color, SDL_Color) const;

  void shift_beginning_down();
  void shift_beginning_up();

  int count_total_lines() const;
  int get_line_length(int) const;
  int get_cursor_pos_on_line() const;

  int get_relative_pos_x(int) const;
  int get_relative_pos_y(int) const;
};


struct tab_t {
  array<buffer_t> buffers;
  buffer_t       *current_buffer = NULL;
  bool            is_used        = false;
};


void draw_tab(const tab_t *, bool);

int number_lines_fits_in_window(const buffer_t *);
int number_chars_on_line_fits_in_window(const buffer_t *);

array<tab_t> &get_tabs();
tab_t       *&get_current_tab();
buffer_t    *&get_current_buffer();
selection_buffer_t &get_selection();

void init(int, char**);
void update();

buffer_t *open_new_buffer(string);
void open_existing_or_new_buffer(string);
void open_existing_buffer(buffer_t *);

tab_t *open_new_tab(string);
void change_tab(s32);

selection_buffer_t *get_selection_buffer();
void delete_selected();
void copy_selected();
void clear_selection();

void paste_from_global_copy();

void go_to_line(int);
void save();

void resize_tab(tab_t*);
void change_buffer(direction_t);
void close_buffer(tab_t*);

void find_in_buffer(string);

int go_word_forward();
int go_word_backwards();

int compute_to_beginning_of_line();
int compute_to_end_of_line();

void undo(buffer_t *);
void redo(buffer_t *);
void save_current_state_for_undo(buffer_t *);

size_t read_file_into_memory(FILE*, char**, size_t gap_len = 0);


inline void copy_window_position(buffer_t *a, const buffer_t *b) { // @CleanUp: can't we just do a memcpy here?
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
