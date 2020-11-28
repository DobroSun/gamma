#include "gamma/pch.h"
#include "gamma/buffer.h"
#include "gamma/init.h"
#include "gamma/font.h"
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
  size_t size_name = 0; \
  for(size_t i = 0; i < arr_size(buffers); i++) { \
    if(buffers[i].is_used) { \
      name[size_name++] = &buffers[i]; \
    } \
  }
#define get_const_buffers(name, size_name, buffers) \
  const buffer_t *name[arr_size(buffers)]; \
  size_t size_name = 0; \
  for(size_t i = 0; i < arr_size(buffers); i++) { \
    if(buffers[i].is_used) { \
      name[size_name++] = &buffers[i]; \
    } \
  }


static selection_buffer_t selection;
static bool is_in_selected(const selection_buffer_t *s, const int ind) {
  return s->start_index <= ind && ind <= s->start_index+(int)s->size;
}
static bool is_last_to_be_selected(const selection_buffer_t *s, const int ind) {
  return ind <= s->start_index+(int)s->size;
}

static tab_t    *active_tab = nullptr;
static buffer_t *active_buffer = nullptr;

static tab_t tabs[12];
static file_buffer_t bufs[48];
static file_buffer_t global_copy_buffer;

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


static void read_entire_file(gap_buffer *ret, FILE *f) {
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


static FILE *get_file_or_create(const char *filename, const char *mods) {
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
  if(FILE *f = fopen(filename.data, "r")) {
    defer { fclose(f); };
    read_entire_file(&buffer->file->buffer, f);
  }
}

void open_existing_buffer(buffer_t *prev) {
  auto buffer = get_free_win_buffer();
  assert(buffer);
  active_buffer = buffer;

  buffer->file     = prev->file;
  buffer->filename = prev->filename;

  buffer->file->buffer.move_until(0);
}

void open_existing_or_new_buffer(literal filename) {
  auto tab = get_current_tab();
  get_used_buffers(used_bufs, size, tab->buffers);

  for(size_t i = 0; i < size; i++) {
    if(used_bufs[i]->filename == filename) {
      open_existing_buffer(used_bufs[i]);
      return;
    }
  }
  open_new_buffer(filename);
}

static void open_tab(literal filename) {
  auto tab = get_free_tab();
  assert(tab);
  active_tab = tab;
  open_new_buffer(filename); // @Incomplete: What if filename is already opened?
}

void tab_t::draw(bool selection_mode) const {
  get_const_buffers(used_bufs, used_size, buffers);

#if 0
  // @Incomplete:
  // Need to update every opened buffer on window.
  // cause right now we don't have partial update.
  //
  const buffer_t *same_buffers[used_size];
  size_t size = 0;
  for(size_t i = 0; i < used_size; i++) {
    if(used_bufs[i]->filename == active_buffer->filename) {
      same_buffers[size++] = used_bufs[i];
    }
  }

  // Updating only the same buffers, as active_buffer.
  for(size_t i = 0; i < size; i++) {
    same_buffers[i]->draw(selection_mode);
  }
#endif

  for(size_t i = 0; i < used_size; i++) {
    used_bufs[i]->draw(selection_mode);
  }

  // Update cursor.
  char s = active_buffer->file->buffer[active_buffer->cursor];
  s = (s == '\n')? ' ': s;
  int px = active_buffer->get_relative_pos_x(active_buffer->n_character-active_buffer->offset_on_line);
  int py = active_buffer->get_relative_pos_y(active_buffer->n_line-active_buffer->start_pos);
  active_buffer->draw_cursor(s, px, py, WhiteColor, BlackColor);

  console_draw();
}

void tab_t::on_resize(int n_width, int n_height) {
  get_used_buffers(used_bufs, size, buffers);

  console_on_resize(n_height);

  for(auto i = 0u; i < size; i++) {
    used_bufs[i]->on_resize(used_bufs[i]->width, used_bufs[i]->height, n_width, get_console()->bottom_y);
  }
}


static bool is_selection = false;
void buffer_t::draw_line(int beginning, int line_number, bool selecting) const {
  if(get_line_length(beginning) <= (int)offset_on_line) { return; }

  const gap_buffer &buffer = file->buffer;
  int i = beginning + offset_on_line, char_number = 0;
  while(buffer[i] != '\n') {
    assert(i >= 0 && (size_t)i < buffer.size());

    const int px = get_relative_pos_x(char_number);
    const int py = get_relative_pos_y(line_number);

    const char c = buffer[i];
    const auto t = get_alphabet()[c];
    assert(t);

    if(px > start_x+width-font_width) {
      return;
    }

    if(selecting) {
      if(is_in_selected(&selection, i)) { is_selection= true; }
      if(is_selection) {
        draw_cursor(c, px, py, WhiteColor, BlackColor);
      } else {
        copy_texture(t, px, py);
      }
      if(is_last_to_be_selected(&selection, i)) { is_selection = false; }

    } else {
      copy_texture(t, px, py);
    }

    i++;
    char_number++;
  }

  // CleanUp: Copy&Paste: of code inside loop.
  assert(i >= 0 && (size_t)i < buffer.size());
  assert(buffer[i] == '\n');

  const int px = get_relative_pos_x(char_number);
  const int py = get_relative_pos_y(line_number);

  const char c = ' ';
  const auto t = get_alphabet()[c];
  assert(t);

  if(px > start_x+width-font_width) {
    return;
  }

  if(selecting) {
    if(is_in_selected(&selection, i)) { is_selection= true; }
    if(is_selection) {
      draw_cursor(c, px, py, WhiteColor, BlackColor);
    } else {
      copy_texture(t, px, py);
    }
    if(is_last_to_be_selected(&selection, i)) { is_selection = false; }

  } else {
    copy_texture(t, px, py);
  }
}

void buffer_t::draw(bool selection_mode) const {
#if 0
  // @Note:
  // When we split on current window and start editing text,
  // it gets wrong on another window if both have different 
  // `start_pos` or `offset_on_line`, so we try to avoid 
  // that right here, deciding to whether update or not, 
  // `another window`. 

  // The problem is current draw function updates every frame,
  // and not when text is changed, so it's impossible to don't
  // get that behavior.
  //
  bool no_update = false;
  if(this != active_buffer) {
    // Handles cases, when we split on current buffer, and trying to edit text
    // in different positions on each window.
    
    if(start_pos > active_buffer->start_pos) { // We don't want to update text below 
      no_update = true;
    }
  }
#endif

  int i = offset_from_beginning, line = 0;
  while((size_t)i < file->buffer.size()-1) {
    assert(i >= 0 && (size_t)i < file->buffer.size()-1);

    if(get_relative_pos_y(line) >= get_console()->bottom_y - font_height) { 
      // If we are out of window.
      break;
    }

    draw_line(i, line, selection_mode);
    i += get_line_length(i);
    line++;
  }

  // @Hack: when inside draw_line we never reach the 
  //  is_selection = false;
  is_selection = false;
}

void buffer_t::draw_cursor(char c, int px, int py, SDL_Color color1, SDL_Color color2) const {
  const char b[2] = {c, '\0'};
  draw_text_shaded(get_font(), reinterpret_cast<const char *>(&b), color1, color2, px, py);
}


int buffer_t::get_relative_pos_x(int n_place) const {
  return start_x + font_width * n_place;
}

int buffer_t::get_relative_pos_y(int n_place) const {
  return start_y + font_height * n_place;
}


void buffer_t::on_resize(int prev_width, int prev_height, int new_width, int new_height) {
  start_x = new_width * start_x / prev_width;
  start_y = new_height * start_y / prev_height;
  width   = new_width * width / prev_width;
  height  = new_height * height / prev_height;

  if(get_relative_pos_y(n_line-start_pos) >= height-font_height) {
    go_up();
  }
}

int buffer_t::get_line_length(int beginning) const {
  int count = 1;
  for(int i = beginning; file->buffer[i] != '\n'; i++) {
    count++;
  }
  return count;
}

void buffer_t::shift_beginning_up() {
  int count = get_line_length(offset_from_beginning);
  offset_from_beginning += count;
  start_pos++;
}

void buffer_t::shift_beginning_down() {
  assert(cursor > 0 && offset_from_beginning > 0);

  int count = 2;
  if(offset_from_beginning < count) {
    assert(offset_from_beginning == 1);

  } else {
    while(file->buffer[offset_from_beginning-count] != '\n') {
      if(count == offset_from_beginning) { count++; break; }
      count++;
    }
  }
  count--;

  offset_from_beginning -= count;
  start_pos--;
}

int buffer_t::get_cursor_pos_on_line() const {
  assert(offset_on_line == 0 && file->buffer[cursor] == '\n');
  if(cursor == 0) return 0;
  
  int count = 1;
  while(file->buffer[cursor-count] != '\n') {
    if(cursor == count) { return count; }
    count++;
  }
  return count-1;
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
    offset_on_line = 0;
    n_line++;
  } else {
    n_character++;
    if(n_character - offset_on_line > number_chars_on_line_fits_in_window(this)) {
      offset_on_line++;
    }
  }
  cursor++;
}

void buffer_t::go_right(bool selecting) {
  if(cursor == file->buffer.size()-1) return;

  file->buffer.move_right();
  inc_cursor();

  if(number_lines_fits_in_window(this) + start_pos == n_line) {  
    shift_beginning_up();
  }

  if(selecting) {
    if(selection.direction == left) {
      selection.start_index = cursor;
      selection.start_line  = n_line;
      selection.start_char  = n_character;

      assert(selection.size > 0);
      selection.size--;

    } else if(selection.direction == right) {
      selection.size++;

    } else {
      assert(selection.direction == none && selection.size == 0);
      selection.direction = right;

      // Copy&Paste: of right case.
      selection.size++;
      assert(selection.size == 1);
    }

    if(selection.size == 0) {
      selection.direction = none;
    }
  }
}


void buffer_t::go_left(bool selecting) {
  if(cursor == 0) return;
  assert(cursor > 0);

  file->buffer.move_left();

  // dec_cursor.
  cursor--;
  if(file->buffer[cursor] == '\n') {
    assert(offset_on_line == 0);
    n_character = get_cursor_pos_on_line();
    n_line--;

    int diff = (int)n_character - number_chars_on_line_fits_in_window(this);
    if(diff > 0 && diff > offset_on_line) {
      offset_on_line = diff;
    }
  } else {
    n_character--;
  }

  if(n_character < offset_on_line) {
    offset_on_line--;
  }
  //

  if(start_pos-1 == n_line && start_pos != 0) {
    shift_beginning_down();
  }

  if(selecting) {
    if(selection.direction == left) {
      selection.start_index = cursor;
      selection.start_line  = n_line;
      selection.start_char  = n_character;
      selection.size++;

    } else if(selection.direction == right) {
      assert(selection.size > 0);
      selection.size--;

    } else {
      assert(selection.direction == none && selection.size == 0);
      selection.direction = left;

      // Copy&Paste: of left case.
      selection.start_index = cursor;
      selection.start_line  = n_line;
      selection.start_char  = n_character;
      selection.size++;
      assert(selection.size == 1);
    }

    if(selection.size == 0) {
      selection.direction = none;
    }
  }
}

void buffer_t::move_to(size_t index) {
  if(cursor < index) {
    size_t diff = index - cursor;
    for(size_t i = 0; i < diff; i++) {
      go_right();
    }

  } else if(cursor > index) {
    size_t diff = cursor - index;
    for(size_t i = 0; i < diff; i++) {
      go_left();
    }

  } else {
    assert(index == cursor);
  }
}

void buffer_t::put_backspace() {
  if(cursor == 0) {
    // Do nothing.
  } else {
    // We don't need to move gap at all,
    // but since go_left calls       gap_buffer.move_left(),
    // we have to compencate it with gap_buffer.move_right().
    file->buffer.move_right();
    go_left();
  }
  file->buffer.backspace();
}

void buffer_t::put_delete() {
  if(cursor == file->buffer.size()-1) return; // @Hack: @CleanUp: on for last character.
  file->buffer.del();
}

void buffer_t::put_return() {
  file->buffer.add('\n');

  go_right();
  file->buffer.move_left(); // Same hack as for ::put_backspace().
  assert(offset_on_line == 0);
}

void buffer_t::put_tab() {
  for(char i = 0; i < tabstop; i++) {
    put(' ');
  }
}

void buffer_t::put(char c) {
  file->buffer.add(c);
  inc_cursor();
}

void buffer_t::go_down(bool selecting) {
  if(n_line == get_total_lines()-1) return;

  auto prev_character_pos = n_character;
  for(size_t i = cursor; file->buffer[i] != '\n'; i++) {
    go_right(selecting);
  }

  go_right(selecting);
  assert(n_character == 0 && offset_on_line == 0);
  
  for(size_t i = 0; i < prev_character_pos; i++) {
    if(file->buffer[cursor] == '\n') break;
    go_right(selecting);
  }
}

void buffer_t::go_up(bool selecting) {
  if(n_line == 0) return;

  size_t prev_character_pos = n_character;
  for(auto i = 0u; i < prev_character_pos; i++) {
    go_left(selecting);
  }
  assert(n_character == 0 && offset_on_line == 0 && n_line > 0);
	go_left(selecting);

  while(n_character > 0) { // go till the beginning of line.
    go_left(selecting);
    if(file->buffer[cursor] == '\n') break;
  }

  for(size_t i = 0; i < prev_character_pos; i++) { // from beginning to actual position.
    if(file->buffer[cursor] == '\n') break;
    go_right(selecting);
  }
}

void buffer_t::scroll_down(bool selecting) {
  if(start_pos == get_total_lines()-1) return;

  if(start_pos == n_line) {
    go_down(selecting);
  }

  shift_beginning_up();
}

void buffer_t::scroll_up(bool selecting) {
  if(offset_from_beginning == 0) return;

  if(number_lines_fits_in_window(this)+start_pos-1 == n_line) {
    go_up(selecting);
  }

  shift_beginning_down();
}

void buffer_t::init(int x, int y, int w, int h) {
  start_x = x; start_y = y; width = w; height = h;
}

int number_lines_fits_in_window(const buffer_t *b) {
  assert(b->height > font_height);
  return b->height / font_height;

}

int number_chars_on_line_fits_in_window(const buffer_t *b) {
  assert(b->width > font_width);
  return b->width / font_width - 1;
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

selection_buffer_t *get_selection_buffer() {
  return &selection;
}

void delete_selected() {
  assert(selection.start_index != -1);
  auto buffer = get_current_buffer();
  buffer->move_to(selection.start_index);

  for(size_t i = 0; i < selection.size+1; i++) {
    buffer->put_delete();
  }
}

void copy_selected() {
  assert(selection.start_index != -1);
  int start = selection.start_index;

  auto &gap_buffer = get_current_buffer()->file->buffer;
  for(size_t i = 0; i < selection.size+1; i++) {
    char c = gap_buffer[start+i];
    global_copy_buffer.buffer.add(c);
  }
  console_put_text("Copied!");
}

void clear_selection() {
  selection.start_index = -1;
  selection.size = 0;
  selection.direction = none;
}

void paste_from_global_copy() {
  auto buffer = get_current_buffer();
  auto &gap_buffer = global_copy_buffer.buffer;

  for(size_t i = 0; i < gap_buffer.size(); i++) {
    buffer->put(gap_buffer[i]);
  }
  console_put_text("Pasted!");
}

void go_to_line(int line) {
  auto buffer = get_current_buffer();
  auto total_lines = buffer->get_total_lines();

  if(line < 0) {
    line = total_lines + line;
    if(line <= 0) {
      line = 1;
    }

  } else if(line > total_lines) {
    line = total_lines;
  } else if(line == 0) {
    line++;
  }

  while(line > (int)buffer->n_line+1) buffer->go_down();
  while(line < (int)buffer->n_line+1) buffer->go_up();
}


void save() {
  auto buffer = get_current_buffer();
  if(buffer->filename.empty()) {
    console_put_text("File has no name.");

  } else {
    assert(!buffer->filename.empty());
    FILE *f = get_file_or_create(buffer->filename.c_str(), "w");
    defer { fclose(f); };

    if(!f) {
      // @Incomplete: report error.
      assert(0);
      print("No file for me (:");
    }

    unsigned i = 0u;
    for( ; i < buffer->file->buffer.size(); i++) {
      fprintf(f, "%c", buffer->file->buffer[i]);
    }
    if(!buffer->file->buffer.size() || buffer->file->buffer[i-1] != '\n') {
      buffer->put('\n');
      buffer->go_left();
      save();
    }
    fflush(f);
    console_put_text("File saved.");
  }
}

void quit(int e) {
  exit(e);
}

static void resize(buffer_t *p, buffer_t *n, split_type_t type) {
  switch(type) {
    case hsp_type: {
      n->init(p->start_x, p->start_y + p->height/2., p->width, p->height/2.);
      p->init(p->start_x, p->start_y,                p->width, p->height/2.);
      break;
    }

    case vsp_type: {
      n->init(p->start_x + p->width/2., p->start_y, p->width/2., p->height);
      p->init(p->start_x,               p->start_y, p->width/2., p->height);
      break;
    }
  }
}


void do_split(const literal &l, split_type_t type) {
  buffer_t *n_buf;
  if(!l.data) {
    // Opening current buffer in another window.

    auto p_buf = get_current_buffer();
    open_existing_buffer(p_buf);
    n_buf = get_current_buffer();

    n_buf->filename = p_buf->filename;
    resize(p_buf, n_buf, type);

  } else {
    // Opening new buffer in another window.
    string_t n_filename = to_string(l);

    auto p_buf = get_current_buffer();
    open_existing_or_new_buffer(to_literal(n_filename));
    n_buf = get_current_buffer();

    n_buf->filename = std::move(n_filename);
    resize(p_buf, n_buf, type);
  }
}

void close_buffer() {
  
}

void cursor_right() {
  auto buffer = get_current_buffer();
  if(buffer->file->buffer[buffer->cursor] == '\n') return; // '\n' means it's the last char on line.
  buffer->go_right();
}

void cursor_left() {
  auto buffer = get_current_buffer();
  if(buffer->cursor == 0) return;
  if(buffer->file->buffer[buffer->cursor-1] == '\n') return;
  buffer->go_left();
}

void cursor_up() {
  get_current_buffer()->go_up();
}

void cursor_down() {
  get_current_buffer()->go_down();
}
