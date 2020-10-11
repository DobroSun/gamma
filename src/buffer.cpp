#include "gamma/pch.h"
#include "gamma/buffer.h"
#include "gamma/init.h"
#include "gamma/font.h"
#include "gamma/input.h"
#include "gamma/console.h"

#include <fcntl.h>

static editor_t editor;

tab_buffer_t &get_current_tab() {
  assert(editor.active_tab);
  return *editor.active_tab;
}

buffer_t &get_current_buffer() {
  auto &tab = get_current_tab();
  assert(tab.active_buffer);
  return *tab.active_buffer;
}

static buffer_t read_entire_file(FILE *f) {
  assert(f);
  buffer_t ret;

  fseek(f, 0, SEEK_END);
  size_t size = ftell(f);

  auto &array = ret.buffer.array;
  auto gap_len = ret.buffer.gap_len;

  array.resize(size+gap_len);

  rewind(f);
  auto res = fread(array.data + gap_len, sizeof(char), size, f);

  if(res != size) {
    fprintf(stderr, "@Incomplete\n");
  }

  return ret;
}

static FILE *get_file_handle(const char *filename, const char *mods) {
  FILE *ret = fopen(filename, mods);
  if(!ret) {
    return nullptr;
  } else {
    return ret;
  }
}

static FILE *get_file_or_create(const char *filename, const char *mods) {
  FILE *ret = get_file_handle(filename, mods);
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

static void close_file(FILE *f) {
  fclose(f);
}

static buffer_t create_buffer_with_name(const char *name) {
  buffer_t ret;
  ret.filename = name;
  return ret;
}

static buffer_t create_buffer_with_no_name() {
  return create_buffer_with_name(nullptr);
}

static buffer_t create_buffer_from_file(const char *filename) {
  if(!filename) {
    return create_buffer_with_no_name();
    
  } else {
    FILE *file = get_file_handle(filename, "r");
    defer { if(file) close_file(file); };

    if(file) {
      return read_entire_file(file);
    } else {
      return create_buffer_with_name(filename);
    }
  }
}

static tab_buffer_t create_tab_from_file(const char *filename) {
  tab_buffer_t tab;
  tab.buffers.push(create_buffer_from_file(filename));
  tab.active_buffer = &tab.buffers[0];
  assert(tab.buffers.size == 1);
  return tab;
}

void tab_buffer_t::draw() const {
  for(auto i = 0u; i < buffers.size; i++) {
    buffers[i].draw();
  }
  console_draw();
}

void tab_buffer_t::on_resize(int n_width, int n_height) {
  console_on_resize(n_height);

  for(auto i = 0u; i < buffers.size; i++) {
    buffers[i].on_resize(buffers[i].width, buffers[i].height, n_width, get_console()->bottom_y);
  }
}


bool buffer_t::is_end_of_line(char c) const {
  return c == '\n';
}

void buffer_t::act_on_non_text_character(int &offset_x, int &offset_y, char c) const {
  if(c == '\n') {
    offset_x = -offset_on_line;
    offset_y++;

  } else if(c == '\t') {
    // @Incomplete:
  }
}

void buffer_t::draw() const {
  // @Hack: with negative numbers, it just works, do I need to change it?
  int offset_x = -offset_on_line, offset_y = 0;
  //print(total_lines);

  unsigned i;
  for(i = offset_from_beginning; i < buffer.size(); i++) {
    int px = get_relative_pos_x(offset_x);
    int py = get_relative_pos_y(offset_y);

    defer { 
      if(i == cursor) {
        char s = buffer[cursor];
        if(is_end_of_line(s)) {
          s = ' ';
        }
        
        draw_cursor(s, px, py, WhiteColor, BlackColor);
      }
    };

    char c = buffer[i];
    act_on_non_text_character(offset_x, offset_y, c);
    if(is_end_of_line(c)) {
      continue;
    }

    auto t = get_alphabet()[c];
    assert(t);

    if(px > width-font_width) {
      continue;
    }
    if(py >= get_console()->bottom_y - font_height) {
      break;
    }

    copy_texture(t, px, py);
    offset_x++;
  }

  if(cursor == buffer.size()) {
    // Cursor is at the end of file.
    
    int px = get_relative_pos_x(offset_x);
    int py = get_relative_pos_y(offset_y);

    char c = ' ';
    draw_cursor(c, px, py, WhiteColor, BlackColor);
  }
}

int buffer_t::get_relative_pos_x(int n_place) const {
  return start_x + font_width * n_place;
}

int buffer_t::get_relative_pos_y(int n_place) const {
  return start_y + font_height * n_place;
}

int buffer_t::get_offset_from_relative_x(int window_x) const {
  return (window_x - start_y) / font_width;
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
  for(auto i = offset_from_beginning; buffer[i] != '\n'; i++) {
    if(i == buffer.size()-2) return true;
  }
  return false;
}

bool buffer_t::is_first_line() const {
  return offset_from_beginning == 0;
}

bool buffer_t::cursor_on_first_line() const {
  auto i = 0u;
  for( ; buffer[i] != '\n'; i++) {
    if(i == cursor) return true;
  }
  if(i == cursor) return true;
  return false;
}

bool buffer_t::cursor_on_last_line() const {
  for(auto i = cursor; buffer[i] != '\n'; i++) {
    if(i == buffer.size()-2) return true;
  }
  return false;
}

unsigned buffer_t::get_cursor_pos_x() const {
  if(cursor == 0) return 0;

  auto tmp = cursor-1;
  auto ret = 0u;
  while(buffer[tmp] != '\n') {
    ret++;

    if(tmp == 0) {
      break;
    } else {
      tmp--;
    }
  }
  return ret;
}

void buffer_t::inc_cursor() {
  if(cursor == buffer.size()) {
    // Do nothing.
  } else {
    if(buffer[cursor] == '\n') {
      n_character = 0;
      n_line++;
    } else {
      n_character++;
    }
    cursor++;
  }
}


void buffer_t::dec_cursor() {
  if(cursor == 0) {
    assert(n_character == 0);
    assert(n_line == 0);
    // Do nothing.
  } else {
    cursor--;
    if(buffer[cursor] == '\n') {
      n_character = get_cursor_pos_x();
      n_line--;
    } else {
      n_character--;
    }
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
  buffer.move_right();
  inc_cursor();
  
  if(get_relative_pos_x(n_character-offset_on_line) > width-font_width) {
    offset_on_line++;
  }

  if(buffer[cursor-1] == '\n') {
    offset_on_line = 0;
  }


  if((height / font_height) + start_pos == n_line) {
    // @CleanUp. @Copy&Paste from scroll down.
    auto count = 0u;
    for(auto i = offset_from_beginning; buffer[i] != '\n'; i++) {
      count++;
    }
    count++; // '\n'.
    // end.

    inc_start(count);
  }

}

void buffer_t::move_left() {
  dec_cursor();

  if(n_character < offset_on_line) {
    offset_on_line--;
  }

  if(buffer[cursor] == '\n') {
    assert(offset_on_line == 0);
    if(get_relative_pos_x(n_character) > width-font_width) {
      unsigned offset = get_offset_from_relative_x(width-font_width);
      assert(n_character > offset);
      offset_on_line = n_character - offset_on_line - offset;
    }
  }

  // @CleanUp.
  if(start_pos-1 == n_line && start_pos != 0) {
    // @CleanUp. @Copy&Paste from scroll_up.
    auto count = 0u;
    if(offset_from_beginning < 2) {
      count++;

    } else {
      for(auto i = offset_from_beginning-2; buffer[i] != '\n'; i--) {
        count++;
        if(i == 0) break;
      }
      count++;
    }

    dec_start(count);
  }
}

void buffer_t::go_left() {
  buffer.move_left();
  move_left();
}

void buffer_t::put_backspace() {
  if(cursor == 0) {
    // Do nothing.
  } else {
    if(buffer[cursor-1] == '\n') total_lines--;

    move_left();
  }
  buffer.backspace();
}

void buffer_t::put_delete() {
  if(buffer[cursor] == '\n') total_lines--;
  buffer.del();
}

void buffer_t::put_return() {
  buffer.add('\n');
  inc_cursor();

  offset_on_line = 0;
  total_lines++;
}

void buffer_t::put(char c) {
  buffer.add(c);

  inc_cursor();

  if(get_relative_pos_x(n_character-offset_on_line) > width-font_width) {
    offset_on_line++;
  }
}

void buffer_t::go_down() {
  if(cursor_on_last_line()) return;

  auto prev_character_pos = n_character;

  for(auto i = cursor; buffer[i] != '\n'; i++) {
    go_right();
  }
  go_right();
  
  for(auto i = 0u; i < prev_character_pos; i++) {
    if(buffer[cursor] == '\n') break;
    go_right();
  }
}

void buffer_t::go_up() {
  if(cursor_on_first_line()) return;

  auto prev_character_pos = n_character;
  for(auto i = 0u; i < prev_character_pos; i++) {
    go_left();
  }
  go_left();

  while(n_character > prev_character_pos) {
    go_left();
    if(buffer[cursor] == '\n') break;
  }
}

void buffer_t::scroll_down() {
  if(is_last_line()) return;

  auto count = 0u;
  for(auto i = offset_from_beginning; buffer[i] != '\n'; i++) {
    count++;
  }
  count++; // '\n'.

  bool cursor_should_go_down = start_pos == n_line;
  if(cursor_should_go_down) {
    go_down();
  }

  inc_start(count);
}

void buffer_t::scroll_up() {
  if(is_first_line()) return;

  auto count = 0u;
  if(offset_from_beginning < 2) {
    count++;

  } else {
    for(auto i = offset_from_beginning-2; buffer[i] != '\n'; i--) {
      count++;
      if(i == 0) break;
    }
    count++;
  }

  bool cursor_should_go_up = (height / font_height) + start_pos - 1 == n_line;
  if(cursor_should_go_up) {
    go_up();
  }
  dec_start(count);
}

void buffer_t::init(const char *f, int x, int y, int w, int h) {
  filename = f; start_x = x; start_y = y; width = w; height = h;

  auto count = 0u;
  for(auto i = 0u; i < buffer.size(); i++) {
    if(buffer[i] == '\n') {
      count++;
    }
  }
  total_lines = count;
}

void buffer_t::save() const {
  if(!filename) {
    console_put_text("File has no name.");

  } else {
    assert(filename);
    FILE *f = get_file_or_create(filename, "w");
    defer { close_file(f); };
    

    if(!f) {
      // @Incomplete.
      print("No file for me (:");
    }

    for(auto i = 0u; i < buffer.size(); i++) {
      fprintf(f, "%c", buffer[i]);
    }
    fflush(f);

    console_put_text("File saved.");
  }
}

void init(int argc, char **argv) {
  const char *filename = nullptr;
  if(argc > 1) {
    filename = argv[1];
  } else {
    // No positional arguments provided.
    // filename == nullptr.
  }

  assert(editor.tabs.size == 0);
  editor.tabs.push(create_tab_from_file(filename));
  editor.active_tab = &editor.tabs[0];
  assert(editor.tabs.size == 1);

  make_font();

  console_init(Height);
  get_current_buffer().init(filename, 0, 0, Width, get_console()->bottom_y);
}

static void update_editor() {
  auto renderer = get_renderer();
  SDL_SetRenderDrawColor(renderer, WhiteColor.r, WhiteColor.g, WhiteColor.b, WhiteColor.a); 
  SDL_RenderClear(renderer);

  get_current_tab().draw();

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

void go_to_line(unsigned line) {
  auto &buffer = get_current_buffer();
  if(line > buffer.total_lines) {
    line = buffer.total_lines;
  } else if(line == 0) {
    line++;
  }

  while(line > buffer.n_line+1) buffer.go_down();
  while(line < buffer.n_line+1) buffer.go_up();
}
