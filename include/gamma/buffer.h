#ifndef GAMMA_BUFFER_H
#define GAMMA_BUFFER_H

struct editor_t {
  dyn_array<tab_buffer_t> tabs;
  tab_buffer_t *active_tab = nullptr;
};

struct tab_buffer_t {
  dyn_array<buffer_t> buffers;

  void draw();
};



struct buffer_t {
  gap_buffer buffer;

  // Position on the window.
  int start_x = 0, start_y = 0;
  int width = Width, height = Height;
  unsigned cursor = 0;


  void draw();
  void act_on_resize(int,int,int,int);
};

tab_buffer_t &get_current_tab();
void init(int, char**);
void update();

#endif
