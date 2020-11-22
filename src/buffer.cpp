#include "gamma/pch.h"
#include "gamma/buffer.h"
#include "gamma/init.h"
#include "gamma/font.h"
#include "gamma/input.h"
#include "gamma/console.h"
#include "gamma/interp.h"

#include <fcntl.h>

// @Speed:
// No need to iterate and get them 
// each time, better solution is to keep 
// an array of currently used bufs.
// @Temporary:
#define get_used_buffers(name, size_name, buffers) \
  buffer_t *name[arr_size(buffers)]; \
  unsigned size_name = 0; \
  for(auto i = 0u; i < arr_size(buffers); i++) { \
    if(buffers[i].is_used) { \
      name[size_name++] = &buffers[i]; \
    } \
  }
#define get_const_buffers(name, size_name, buffers) \
  const buffer_t *name[arr_size(buffers)]; \
  unsigned size_name = 0; \
  for(auto i = 0u; i < arr_size(buffers); i++) { \
    if(buffers[i].is_used) { \
      name[size_name++] = &buffers[i]; \
    } \
  }


static tab_t    *active_tab = nullptr;
static buffer_t *active_buffer = nullptr;

static tab_t tabs[12];
static file_buffer_t bufs[48];


static file_buffer_t *get_free_file_buffer() {
  for(size_t i = 0; i < arr_size(bufs); i++) {
    if(!bufs[i].is_used) {
      bufs[i].is_used = true;
      return &bufs[i];
    }
  }
  // @Incomplete: report error.
  return nullptr;
}

static buffer_t *get_free_win_buffer() {
  for(size_t i = 0; i < arr_size(active_tab->buffers); i++) {
    auto &buf = active_tab->buffers[i];
    if(!buf.is_used) {
      buf.is_used = true;
      return &buf;
    }
  }
  // @Incomplete: report error.
  return nullptr;
}

static tab_t *get_free_tab() {
  for(size_t i = 0; i < arr_size(tabs); i++) {
    if(!tabs[i].is_used) {
      tabs[i].is_used = true;
      return &tabs[i];
    }
  }
  // @Incomplete: report error.
  return nullptr;
}

#if 0
void finish_file_buffer(file_buffer *f) {
  f->is_used = false;
  //dealloc(f->buffer);
}
void finish_win_buffer(buffer_tt *b) {
  b->is_used = false;
  finish_file_buffer(b->file);
}
#endif


tab_t *get_current_tab() {
  return active_tab;
}

buffer_t *get_current_buffer() {
  return active_buffer;
}


void read_entire_file(gap_buffer *ret, FILE *f) {
  assert(f);
  assert(ret);

  fseek(f, 0, SEEK_END);
  size_t size = ftell(f);

  ret->chars.resize_with_no_init(size + ret->gap_len);

  rewind(f);
  auto res = fread(ret->chars.data + ret->gap_len, sizeof(char), size, f);

  if(res != size) {
    fprintf(stderr, "@Incomplete\n");
  }
}


FILE *get_file_or_create(const char *filename, const char *mods) {
  FILE *ret = fopen(filename, mods);
  if(!ret) {
    int fd = open(filename, O_RDWR | O_CREAT, 0);
    if(!fd) {
      // @Temporary.
    }
    assert(fd);
    return fopen(filename, mods);

  } else {
    return ret;
  }
}


void open_new_buffer(literal filename) {
  auto buffer = get_free_win_buffer();
  assert(buffer);
  active_buffer = buffer;

  buffer->file = get_free_file_buffer();
  assert(buffer->file);

  if(!filename.data) return;
  {
    if(FILE *f = fopen(filename.data, "r")) {
      defer { fclose(f); };
      read_entire_file(&buffer->file->buffer, f);
    }
  }
}

void open_existing_buffer(buffer_t *prev) {
  auto buffer = get_free_win_buffer();
  assert(buffer);
  active_buffer = buffer;

  buffer->file     = prev->file;
  buffer->filename = prev->filename;
}

void open_existing_or_new_buffer(literal filename) {
  auto tab = get_current_tab();
  get_used_buffers(used_bufs, size, tab->buffers);

  for(size_t i = 0; i < size; i++) {
    if(used_bufs[i]->filename == filename) {
      auto buffer = get_free_win_buffer();
      assert(buffer);
      active_buffer = buffer;

      buffer->file = used_bufs[i]->file;
      assert(buffer->file);
      return;
    }
  }
  open_new_buffer(filename);
}

static void open_tab(literal filename) {
  auto tab = get_free_tab();
  assert(tab);
  active_tab = tab;
  open_new_buffer(filename);
}

void tab_t::draw() const {
  get_const_buffers(used_bufs, size, buffers);

  for(auto i = 0u; i < size; i++) {
    used_bufs[i]->draw();
  }
  console_draw();
}

void tab_t::on_resize(int n_width, int n_height) {
  get_used_buffers(used_bufs, size, buffers);

  console_on_resize(n_height);

  for(auto i = 0u; i < size; i++) {
    used_bufs[i]->on_resize(used_bufs[i]->width, used_bufs[i]->height, n_width, get_console()->bottom_y);
  }
}


#if 0
void buffer_t::act_on_non_text_character(int &offset_x, int &offset_y, char c) const {
  if(c == '\n') {
    offset_x = -offset_on_line;
    offset_y++;

  } else if(c == '\t') {
    // @Incomplete:
  }
}
#endif

int buffer_t::get_line_length(int beginning) const {
  int count = 1;
  for(int i = beginning; file->buffer[i] != '\n'; i++) {
    count++;
  }
  return count;
}

void buffer_t::draw_line(int beginning, int line_number) const {
  if(get_line_length(beginning) <= offset_on_line) { return; }

  gap_buffer &buffer = file->buffer;
  for(int i = beginning+offset_on_line, char_number = 0; buffer[i] != '\n'; i++, char_number++) {
    int px = get_relative_pos_x(char_number);
    int py = get_relative_pos_y(line_number);

    char c = buffer[i];
    auto t = get_alphabet()[c];
    assert(t);

    if(px > start_x+width-font_width) {
      break;
    }

    copy_texture(t, px, py);
  }

}

void buffer_t::draw() const {
  int i = offset_from_beginning, line = 0;
  while(i < file->buffer.size()) {
    if(get_relative_pos_y(line) >= get_console()->bottom_y - font_height) { 
      // If is out of window.
      break;
    }

    draw_line(i, line);
    i += get_line_length(i);
    line++;

  }

  //assert(n_line <= line);

  // Draw cursor.
  char s = file->buffer[cursor];
  if(s == '\n') { s = ' '; }

  int px = get_relative_pos_x(n_character);
  int py = get_relative_pos_y(n_line);
  draw_cursor(s, px, py, WhiteColor, BlackColor);

#if 0
  // @Hack: with negative numbers, it just works, do I need to change it?
  int offset_x = -offset_on_line, offset_y = 0;

  unsigned i;
  for(i = offset_from_beginning; i < file->buffer.size(); i++) {
    int px = get_relative_pos_x(offset_x);
    int py = get_relative_pos_y(offset_y);


    defer { 
      if(i == cursor) {
        char s = file->buffer[cursor];
        if(s == '\n') {
          s = ' ';
        }
        
        draw_cursor(s, px, py, WhiteColor, BlackColor);
      }
    };

    char c = file->buffer[i];
    act_on_non_text_character(offset_x, offset_y, c);
    if(c == '\n') {
      continue;
    }

    auto t = get_alphabet()[c];
    assert(t);

    if(px > start_x+width-font_width) {
      continue;
    }
    if(py >= get_console()->bottom_y - font_height) {
      break;
    }

    copy_texture(t, px, py);
    offset_x++;
  }

  if(cursor == file->buffer.size()) {
    // Cursor is at the end of file.
    
    int px = get_relative_pos_x(offset_x);
    int py = get_relative_pos_y(offset_y);

    char c = ' ';
    draw_cursor(c, px, py, WhiteColor, BlackColor);
  }
#endif
}

int buffer_t::get_relative_pos_x(int n_place) const {
  return start_x + font_width * n_place;
}

int buffer_t::get_relative_pos_y(int n_place) const {
  return start_y + font_height * n_place;
}

void buffer_t::draw_cursor(char c, int px, int py, SDL_Color color1, SDL_Color color2) const {
  const char b[2] = {c, '\0'};
  draw_text_shaded(get_font(), reinterpret_cast<const char *>(&b), color1, color2, px, py);
}


void buffer_t::on_resize(int prev_width, int prev_height, int new_width, int new_height) {
  start_x = new_width * start_x / prev_width;
  start_y = new_height * start_y / prev_height;
  width   = new_width * width / prev_width;
  height  = new_height * height / prev_height;
}

bool buffer_t::is_last_line() const {
  for(auto i = offset_from_beginning; file->buffer[i] != '\n'; i++) {
    if(i == file->buffer.size()-2) return true;
  }
  return false;
}

bool buffer_t::is_first_line() const {
  return offset_from_beginning == 0;
}

bool buffer_t::cursor_on_first_line() const {
  if(cursor == 0) return true;
  unsigned i = 0;
  for( ; file->buffer[i] != '\n'; i++) {
    if(i == cursor) return true;
  }
  if(i == cursor) return true;
  return false;
}

bool buffer_t::cursor_on_last_line() const {
  if(cursor == file->buffer.size()) return true;
  size_t i = cursor;
  for( ; file->buffer[i] != '\n'; i++) {
    if(i == file->buffer.size()-1) return true;
  }
  if(i == file->buffer.size()-1) return true;
  return false;
}

bool buffer_t::char_fits_on_buffer_width() const {
  return n_character - offset_on_line <= (unsigned)num_chars_fits_to_buffer_width();
}

int buffer_t::num_chars_fits_to_buffer_width() const {
  return width/font_width - 1;
}

int buffer_t::num_to_shift_down_on_scrolling() const {
  int count = 0;
  for(auto i = offset_from_beginning; file->buffer[i] != '\n'; i++) {
    count++;
  }
  count++; // '\n'.
  return count;
}

int buffer_t::num_to_shift_up_on_scrolling() const {
  int count = 0;
  if(offset_from_beginning < 2) {
    count++;

  } else {
    for(auto i = offset_from_beginning-2; file->buffer[i] != '\n'; i--) {
      count++;
      if(i == 0) break;
    }
    count++;
  }
  return count;
}

int buffer_t::get_cursor_pos_x() const {
  if(cursor == 0) return 0;

  int tmp = cursor-1;
  int ret = 0;
  while(file->buffer[tmp] != '\n') {
    ret++;

    if(tmp == 0) {
      break;
    } else {
      tmp--;
    }
  }
  return ret;
}

int buffer_t::get_total_lines() const {
  int count = 0;
  for(auto i = 0u; i < file->buffer.size(); i++) {
    if(file->buffer[i] == '\n') count++;
  }
  return count;
}

void buffer_t::inc_cursor() {
  if(file->buffer[cursor] == '\n') {
    n_character = 0;
    n_line++;
  } else {
    n_character++;
  }
  cursor++;
}


void buffer_t::dec_cursor() {
  cursor--;
  if(file->buffer[cursor] == '\n') {
    n_character = get_cursor_pos_x();
    n_line--;
  } else {
    n_character--;
  }
}

void buffer_t::inc_start(int n) {
  offset_from_beginning += n;
  start_pos++;
}

void buffer_t::dec_start(int n) {
  offset_from_beginning -= n;
  start_pos--;
}

void buffer_t::go_right() {
  if(cursor == file->buffer.size()) return;
  file->buffer.move_right();
  inc_cursor();
  
  if(!char_fits_on_buffer_width()) {
    offset_on_line++;
  }

  if(file->buffer[cursor-1] == '\n') {
    offset_on_line = 0;
  }

  if((height / font_height) + start_pos == n_line) {  
    int shift = num_to_shift_down_on_scrolling();
    inc_start(shift);
  }
}


void buffer_t::move_left() {
  if(cursor == 0) return;
  dec_cursor();

  if(n_character < offset_on_line) {
    offset_on_line--;
  }

  if(file->buffer[cursor] == '\n') {
    assert(offset_on_line == 0);
    if(!char_fits_on_buffer_width()) {
      offset_on_line = n_character - num_chars_fits_to_buffer_width();
    }
  }

  if(start_pos-1 == n_line && start_pos != 0) {
    int shift = num_to_shift_up_on_scrolling();
    dec_start(shift);
  }
}

void buffer_t::go_left() {
  file->buffer.move_left();
  move_left();
}

void buffer_t::put_backspace() {
  if(cursor == 0) {
    // Do nothing.
  } else {
    move_left();
  }
  file->buffer.backspace();
}

void buffer_t::put_delete() {
  file->buffer.del();
}

void buffer_t::put_return() {
  file->buffer.add('\n');
  inc_cursor();

  offset_on_line = 0;
}

void buffer_t::put(char c) {
  file->buffer.add(c);
  inc_cursor();
  if(!char_fits_on_buffer_width()) {
    offset_on_line++;
  }
}

void buffer_t::go_down() {
  if(cursor_on_last_line()) return;

  auto prev_character_pos = n_character;
  for(auto i = cursor; file->buffer[i] != '\n'; i++) {
    go_right();
  }
  go_right();
  
  for(auto i = 0u; i < prev_character_pos; i++) {
    if(cursor == file->buffer.size() || file->buffer[cursor] == '\n') break;
    go_right();
  }
}

void buffer_t::go_up() {
  if(cursor_on_first_line()) return;

  auto prev_character_pos = n_character;
  for(auto i = 0u; i < prev_character_pos; i++) {
    go_left();
  }

  assert(n_character == 0 && offset_on_line == 0);
  go_left();

  while(n_character > 0) {
    go_left();
    if(file->buffer[cursor] == '\n') break;
  }

  while(n_character < prev_character_pos) {
    if(file->buffer[cursor] == '\n') break;
    go_right();
  }
}

void buffer_t::scroll_down() {
  if(is_last_line()) return;
   
  int shift = num_to_shift_down_on_scrolling();

  if(start_pos == n_line) {
    go_down();
  }

  inc_start(shift);
}

void buffer_t::scroll_up() {
  if(is_first_line()) return;


  bool cursor_should_go_up = (height / font_height) + start_pos - 1 == n_line;
  if(cursor_should_go_up) {
    go_up();
  }

  int shift = num_to_shift_up_on_scrolling();
  dec_start(shift);
}

void buffer_t::init(int x, int y, int w, int h) {
  start_x = x; start_y = y; width = w; height = h;
}

void init(int argc, char **argv) {
  init_var_table();

  string_t filename;
  if(argc > 1) {
    filename = argv[1];
  } else {
    // No positional arguments provided.
    // filename.data == nullptr.
  }

  open_tab(to_literal(filename));
  make_font();

  console_init(Height);

  auto buffer = get_current_buffer();

  buffer->filename = filename;
  get_current_buffer()->init(0, 0, Width, get_console()->bottom_y);
}

static void update_editor() {
  auto renderer = get_renderer();
  SDL_SetRenderDrawColor(renderer, WhiteColor.r, WhiteColor.g, WhiteColor.b, WhiteColor.a); 
  SDL_RenderClear(renderer);

  get_current_tab()->draw();

  SDL_RenderPresent(renderer);
}

static void update_console() {
  draw_rect(0, get_console()->bottom_y, Width, font_height, WhiteColor);
  console_draw();

  SDL_RenderPresent(get_renderer());
}

void update() {
  switch(*get_editor_mode()) {
    case EditorMode::Editor: {
      update_editor();
    } break;

    case EditorMode::Console: {
      update_console();
    } break;
  }
}
