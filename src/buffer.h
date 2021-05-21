#ifndef GAMMA_BUFFER_H
#define GAMMA_BUFFER_H


struct Select_Buffer {
  size_t first, last;
};

struct Buffer_Component {
  gap_buffer buffer;
  size_t cursor1; // @Rename: 
  size_t n_character, n_line;
  size_t offset_from_beginning, start_pos;
  size_t indentation_level;
  size_t total_lines;

  int start_x, start_y, width, height; // @Ugh: 


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

  int get_relative_pos_x(int) const;
  int get_relative_pos_y(int) const;

  // Helpers.
  void go_to(size_t);
  void go_right();
  void go_left();
  void go_down();
  void go_up();
};

struct Undo_Component {
  array<Buffer_Component> undo;
  array<Buffer_Component> redo;
};

struct Loc {
  size_t index, l, c, size;
};

struct Search_Component {
  array<Loc> found;
  size_t search_index;
  bool found_in_a_file;
};

struct buffer_t {
  string filename;

  Buffer_Component buffer_component;
  Search_Component search_component;
  Undo_Component   undo_component;


  size_t offset_on_line; // @ActuallyNotUsed: @RemoveMe: 

  void draw() const;
};

struct tab_t {
  array<buffer_t> buffers;
  buffer_t *current_buffer;
};


int number_lines_fits_in_window(const Buffer_Component *);
int number_chars_on_line_fits_in_window(const Buffer_Component *);
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
void yield_selected(gap_buffer, Select_Buffer);
void paste_from_buffer(Buffer_Component*);

void go_to_line(int);
void save();

void resize_tab(tab_t *);
void change_buffer(s32);
void close_buffer(tab_t*);

void to_next_in_search(Search_Component *, Buffer_Component *);
void to_prev_in_search(Search_Component *, Buffer_Component *);
void find_in_buffer(Search_Component *, Buffer_Component *, const string);

size_t read_file_into_memory(FILE*, char**, size_t gap_len = 0);

void go_word_forward();
void go_word_backwards();
void go_paragraph_forward();
void go_paragraph_backwards();

// 
void no_action(Buffer_Component *buffer);
void select_action(Buffer_Component *buffer);
void select_line_action(Buffer_Component *buffer);
void delete_action(Buffer_Component *buffer);
void yield_action(Buffer_Component *buffer);

void select_to_left(Buffer_Component *buffer);
void select_to_right(Buffer_Component *buffer);
void select_line_to_left(Buffer_Component *buffer);
void select_line_to_right(Buffer_Component *buffer);
// 



void save_current_state_for_undo(Undo_Component *, Buffer_Component *);
void undo(Undo_Component *, Buffer_Component *);
void redo(Undo_Component *, Buffer_Component *);

Buffer_Component to_beginning_of_line(Buffer_Component);
Buffer_Component to_end_of_line(Buffer_Component);
Buffer_Component move_to(Buffer_Component);

void update_indentation_level(buffer_t*);

#endif
