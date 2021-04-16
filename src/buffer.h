#ifndef GAMMA_BUFFER_H
#define GAMMA_BUFFER_H



struct selection_buffer_t { size_t first = 0, last = 0; };
struct Loc { size_t index, l, c, size; };


struct buffer_t {
  gap_buffer      buffer;
  array<buffer_t> undo;
  array<buffer_t> redo;

  string filename;
  array<Loc> found; // search.

  int start_x, start_y, width, height;
  size_t cursor = 0, n_character = 0, n_line = 0, total_lines = 0;
  size_t offset_on_line = 0, offset_from_beginning = 0, start_pos = 0, saved_pos = 0;


  void draw() const;
  void save();

  // @CleanUp all cursor functions.
  void scroll_down();
  void scroll_up();
  void go_right();
  void go_left();
  void go_down();
  void go_up();
  void put_backspace();
  void put_return();
  void put_delete();
  void put(char);
  void put_tab();


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
  buffer_t *current_buffer = NULL;
};


int number_lines_fits_in_window(const buffer_t *);
int number_chars_on_line_fits_in_window(const buffer_t *);
int get_current_line_indent(buffer_t *);

array<tab_t> &get_tabs();
tab_t       *&get_current_tab();
buffer_t    *&get_current_buffer();
selection_buffer_t &get_selection();

void init(int, char**);

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

void resize_tab(tab_t *);
void change_buffer(s32);
void close_buffer(tab_t*);

void find_in_buffer(string);

int compute_to_beginning_of_line();
int compute_to_end_of_line();

size_t read_file_into_memory(FILE*, char**, size_t gap_len = 0);

void go_to_beginning_of_line();
void go_to_end_of_line();

void go_word_forward();
void go_word_backwards();
void go_paragraph_forward();
void go_paragraph_backwards();

void no_action(buffer_t*);
void select_action(buffer_t*);
void delete_action(buffer_t*);

void save_current_state_for_undo(buffer_t *);
void undo(buffer_t *);
void redo(buffer_t *);

#endif
