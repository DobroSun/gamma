#ifndef GAMMA_BUFFER_H
#define GAMMA_BUFFER_H

tab_buffer_t &get_current_tab();
buffer_t &get_current_buffer();


struct editor_t {
  dyn_array<tab_buffer_t> tabs;
  tab_buffer_t *active_tab = nullptr;
};

struct tab_buffer_t {
  dyn_array<buffer_t> buffers;
  buffer_t *active_buffer = nullptr;

  void draw() const;
  void on_resize(int,int);
};


struct buffer_t {
  gap_buffer buffer;
  const char *filename;


  // Position on the window.
  int start_x = 0, start_y = 0, width = Width, height = Height;
  unsigned cursor = 0, n_character = 0, n_line = 0;
  unsigned offset_on_line = 0, offset_from_beginning = 0, start_pos = 0;
  unsigned total_lines = 0; // total_#_of_characters is buffer.size().


  void init(const char*,int,int,int,int);
  void draw() const;
  void save() const;
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

private:
  void draw_cursor(char, int, int, SDL_Color, SDL_Color) const;
  bool is_last_line() const;
  bool is_first_line() const;
  bool cursor_on_last_line() const;
  bool cursor_on_first_line() const;

  int get_relative_pos_x(int) const;
  int get_relative_pos_y(int) const;

  int get_offset_from_relative_x(int) const;

  unsigned get_cursor_pos_x() const;

  void inc_cursor();
  void dec_cursor();

  void inc_start(int);
  void dec_start(int);

  void act_on_non_text_character(int&, int&, char) const;
  bool is_end_of_line(char) const;
};

void init(int, char**);
void update();

void go_to_line(unsigned);
#endif
