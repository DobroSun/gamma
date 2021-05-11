#ifndef GAMMA_BUFFER_H
#define GAMMA_BUFFER_H


struct Select_Buffer {
  size_t first, last;
};

struct Loc { size_t index, l, c, size; };

struct buffer_t {
  gap_buffer buffer;
  array<buffer_t> undo;
  array<buffer_t> redo;

  string filename;

  // search.
  array<Loc> found;
  bool found_in_a_file;
  size_t search_index;
  // 

  int start_x, start_y, width, height;

  
  size_t n_character, n_line, offset_from_beginning, start_pos;

  size_t offset_on_line, total_lines;

  size_t indentation_level;


  void draw() const;

  // 
  size_t to_left(size_t);
  size_t to_right(size_t);
  size_t to_down(size_t);
  size_t to_up(size_t);
  size_t to_end_of_line(size_t);
  size_t to_beginning_of_line(size_t);
  void   move_to(size_t);
  void   go_to(size_t);

  size_t get_line_length(size_t) const;
  size_t count_all_lines() const;

  void shift_beginning_down();
  void shift_beginning_up();
  void scroll_down();
  void scroll_up();

  void put_backspace();
  void put_return();
  void put_delete();
  void put(char);
  void put_tab();

  size_t cursor() const;
  char getchar(size_t) const;
  bool start(size_t) const;
  bool eol(size_t) const;
  bool eof(size_t) const;
  char getchar() const;
  bool start() const;
  bool eol() const;
  bool eof() const;
  // 


  // Helpers.
  void go_right();
  void go_left();
  void go_down();
  void go_up();
  // 

  int get_relative_pos_x(int) const;
  int get_relative_pos_y(int) const;
};


struct tab_t {
  array<buffer_t> buffers;
  buffer_t *current_buffer;
};


int number_lines_fits_in_window(const buffer_t *);
int number_chars_on_line_fits_in_window(const buffer_t *);
int get_current_line_indent(buffer_t *);

array<tab_t> &get_tabs();
tab_t       *&get_current_tab();
buffer_t    *&get_current_buffer();
Select_Buffer &get_selection();

void init(int, char**);

buffer_t *open_new_buffer(string);
void open_existing_or_new_buffer(string);
void open_existing_buffer(buffer_t *);

tab_t *open_new_tab(string);
void change_tab(s32);

void delete_selected(buffer_t *);
void yield_selected(buffer_t *);
void paste_from_buffer(buffer_t *);

void go_to_line(int);
void save();

void resize_tab(tab_t *);
void change_buffer(s32);
void close_buffer(tab_t*);

void to_next_in_search();
void to_prev_in_search();
void find_in_buffer(const string);

size_t read_file_into_memory(FILE*, char**, size_t gap_len = 0);

void go_word_forward();
void go_word_backwards();
void go_paragraph_forward();
void go_paragraph_backwards();

void no_action(buffer_t*);
void select_action(buffer_t*);
void delete_action(buffer_t*);
void yield_action(buffer_t*);
void select_to_left(buffer_t*);
void select_to_right(buffer_t*);

void save_current_state_for_undo(buffer_t *);
void undo(buffer_t *);
void redo(buffer_t *);

void update_indentation_level(buffer_t*);

#endif
