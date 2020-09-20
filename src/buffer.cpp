#include "gamma/pch.h"
#include "gamma/buffer.h"
#include "gamma/init.h"
#include "gamma/font.h"


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

static char *read_args(int argc, char **argv) {
  if(argc > 1) {
    return argv[1];
  } else {
    assert(0);
    return argv[0];
  }
}

static buffer_t read_entire_file(const char *filename) {
  buffer_t ret;

  FILE* f = fopen(filename, "r");
  defer { if(f) fclose(f); };
  if(!f) {
    fprintf(stderr, "Error opening file: \"%s\".\n", filename);
    assert(0);
    return ret;
  }

  fseek(f, 0, SEEK_END);
  size_t size = ftell(f);

  auto &array = ret.buffer.array;
  auto gap_len = ret.buffer.gap_len;

  {
  auto size_with_gap = size + gap_len;
	array.data = new char[size_with_gap];
	array.capacity = size_with_gap;
	array.size = size_with_gap;
  }

  rewind(f);
  auto res = fread(array.data + gap_len, sizeof(char), size, f);

  if(res != size) {
    fprintf(stderr, "@Incomplete\n");
  }

  return ret;
}

static buffer_t create_buffer_from_file(const char *filename) {
  // if(filename `path doesn't exist`) {
  //   open_empty_file();
  // }
  return read_entire_file(filename);
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
}


// @CleanUp.
static int tw, th;
static void copy_texture(SDL_Texture *t, int px, int py) {
  SDL_QueryTexture(t, nullptr, nullptr, &tw, &th);
  SDL_Rect dst {px, py, tw, th};
  SDL_RenderCopy(get_renderer(), t, nullptr, &dst);
}

void buffer_t::draw() const {
  int offset_x = -offset_on_line, offset_y = 0;

  for(auto i = offset_from_beginning; i < buffer.size(); i++) {
    int px = get_relative_pos_x(offset_x);
    int py = get_relative_pos_y(offset_y);

    defer { if(i == cursor) draw_cursor(px, py, WhiteColor, BlackColor); };

    char c = buffer[i];
    if(c == '\n') {
      offset_x = -offset_on_line;
      offset_y++;
      continue;

    } else if(c == '\t') {
      // @Incomplete:
    }

    auto t = get_alphabet()[c];
    assert(t);

    if(px > width-font_width) {
      continue;
    }

    if(py > height) {
      break;
    }
    
    copy_texture(t, px, py);
    offset_x++;
  }
}

int buffer_t::get_relative_pos_x(int n_place) const {
  return start_x + font_width * n_place;
}

int buffer_t::get_relative_pos_y(int n_place) const {
  return start_y + font_height * n_place;
}

void buffer_t::draw_cursor(int px, int py, SDL_Color color1, SDL_Color color2) const {
  char c = (buffer[cursor] == '\n')? ' ': buffer[cursor];
  draw_text_shaded(get_font(), &c, color1, color2, px, py);
}

void buffer_t::act_on_resize(int prev_width, int prev_height, int new_width, int new_height) {
  start_x = new_width * start_x / prev_width;
  start_y = new_height * start_y / prev_height;
  width   = new_width * width / prev_width;
  height  = new_height * height / prev_height;
  assert(start_x <= width);
  assert(start_y <= height);
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
  if(cursor == buffer.size()-1) {
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
  
  if(get_relative_pos_x(n_character) > width-font_width) {
    offset_on_line++;
  }

  if(buffer[cursor-1] == '\n') {
    offset_on_line = 0;
  }
}

void buffer_t::go_left() {
  buffer.move_left();
  dec_cursor();

  if(n_character < offset_on_line) {
    offset_on_line--;
  }

  if(buffer[cursor] == '\n') {
    while(get_relative_pos_x(n_character) > width-font_width) {
      dec_cursor();
    }

    while(buffer[cursor] != '\n') {
      go_right();
    }
  }
}

void buffer_t::put_backspace() {
  buffer.backspace();
  if(cursor == 0) {
    // Do nothing.
  } else {
    cursor--;
  }
}

void buffer_t::put_delete() {
  buffer.del();
}

void buffer_t::put_return() {
  buffer.add('\n');
  cursor++;
}

void buffer_t::put_key(char c) {
  buffer.add(c);
  cursor++;
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


void init(int argc, char **argv) {
  const char *filename = read_args(argc, argv);

  editor.tabs.push(create_tab_from_file(filename));
  editor.active_tab = &editor.tabs[0];
  assert(editor.tabs.size == 1);

  make_font();
}

void update() {
  SDL_SetRenderDrawColor(get_renderer(), 255, 255, 255, 255); 
  SDL_RenderClear(get_renderer());

  get_current_tab().draw();

  SDL_RenderPresent(get_renderer());
}

#if 0
static void fix_gap() {
  auto &cursor = buffer.cursor;
  auto &line = buffer[cursor.i];
  int diff = cursor.j - line.pre_len;
  if(diff > 0) {
    // cursor.j is bigger than start of gap.
    // so moving gap right.
    line.move_right_by(diff);
  
  } else if(diff < 0) {
    // cursor.j is less than start of gap.
    line.move_left_by(-diff);

  } else {
    assert(!diff);
    // There is no difference between cursor.j and start index of gap.
    // do nothing.
  } 
}

static void cursor_right_detail() {
  auto &cursor = buffer.cursor;
  auto &start_j = buffer.start_j;
  auto &i = cursor.i; auto &j = cursor.j;
  auto &buffer_i = buffer[i];

  const int max_line = (start_j+1) * buffer_width() / fw;
  if(j == max_line-1) {
    start_j++;
  }

  if(j < (int)buffer_i.size()-1) {
    buffer_i.move_right();
    j++;
  }
}

static void cursor_right() {
  cursor_right_detail();
  buffer.saved_j = buffer.cursor.j;
}

static void cursor_right_no_move() {
  auto &cursor = buffer.cursor;
  auto &start_j = buffer.start_j;
  auto &j = cursor.j;

  const int max_line = (start_j+1) * buffer_width() / fw;
  if(j == max_line-1) {
    start_j++;
  }

  j++;
  buffer.saved_j = j;
}

static void cursor_left_detail() {
  auto &cursor = buffer.cursor;
  auto &start_j = buffer.start_j;
  auto &i = cursor.i; auto &j = cursor.j;
  auto &buffer_i = buffer[i];

  const int max_line = start_j * buffer_width() / fw;
  if(j && j == max_line) {
    start_j--;
  }

  if(j > (int)0) {
    buffer_i.move_left();
    j--;
  }
}

static void cursor_left() {
  cursor_left_detail();
  buffer.saved_j = buffer.cursor.j;
}

static void cursor_to(int to_i, int to_j) {
  auto &cursor = buffer.cursor;
  auto &i = cursor.i; auto &j = cursor.j;
  if(to_i == i) {
    int diff = to_j - j;
    while(diff > 0) {
      cursor_right_detail();
      diff--;
    } 
    while(diff < 0) {
      cursor_left_detail();
      diff++;
    } 
    assert(to_j == j);
  }
}

static void cursor_down() {
  auto &start = buffer.start;
  auto &cursor = buffer.cursor;
  auto &i = cursor.i;
  const int max_size = buffer.size()-1;

  if(i == max_size) { // On the last line of file.  
    return;
  } 
  
  if((int)(i-start) == numrows()-1) { // On the last line of page.  
    start++;
  }

  buffer.move_right();
  i++;
  cursor_to(i, std::min(buffer.saved_j, buffer[i].size()-1));
  fix_gap();
}

static void cursor_up() {
  auto &start = buffer.start;
  auto &cursor = buffer.cursor;
  auto &i = cursor.i; //auto &j = cursor.j;

  if(i == 0) { // On the first line of file.  
    return;
  }

  if((int)(i-start) == 0) { // On the firtst line of page.
    start--;
  }


  buffer.move_left();
  i--;
  cursor_to(i, std::min(buffer.saved_j, buffer[i].size()-1));
  fix_gap();
}

static void return_key() {
  auto &start = buffer.start;
  auto &cursor = buffer.cursor;
  auto &i = cursor.i; auto &j = cursor.j;
  auto buf_i = buffer[i];

  gap_buffer<char> to_end;
  gap_buffer<char> from_start;

  for(auto k = 0; k < j; k++) {
    from_start.add(buf_i[k]);
  }
  from_start.add(' '); // add extra space.
  for(unsigned k = j; k < buf_i.size(); k++) {
    to_end.add(buf_i[k]);
  }
  from_start.move_left_by(from_start.size());
  to_end.move_left_by(to_end.size());

  buffer.del();
  buffer.add(from_start);
  buffer.add(to_end);

  if((int)(i-start) == numrows()-1) {
    start++;
  }

  i++;
  buffer.move_left();
  cursor_to(i, 0);
  buffer.saved_j = j;
  fix_gap();
}

static void backspace_key() {
  auto &cursor = buffer.cursor;
  auto &start = buffer.start;
  auto &i = cursor.i; auto &j = cursor.j;

  if(j != 0) {
    j--;
    buffer[i].backspace();
  } else {
    if(!i) return;
    assert(j == 0);
    
    auto &previous_line = buffer[i-1];
    auto current_line = buffer[i];
    auto size = previous_line.size()-1;
    string s = "";
    for(unsigned k = 0; k < current_line.size(); k++) {
      if(k == 0) { // overwriting trailing space.
        assert(previous_line[size] == ' ');
        previous_line[size] = current_line[k];
      } else {     // inserting to the end.
        s.push_back(current_line[k]);
      }
    }
    previous_line.insert_many(s);

    if((int)(i-start) == 0) {
      start--;
    }

    i--;
    buffer.del();

    buffer.move_left();
    cursor_to(i, size);
  }
  buffer.saved_j = j;
  fix_gap();
}

static void delete_key() {
  auto &cursor = buffer.cursor;
  auto &start = buffer.start;
  auto &i = cursor.i; auto &j = cursor.j;

  if((unsigned)j != buffer[i].size()-1) {
    buffer[i].del();
  } else {
    if((unsigned)i == buffer.size()-1) return;


    auto next_line = buffer[i+1];
    auto &current_line = buffer[i];
    auto size = current_line.size()-1;
    string s = "";
    for(unsigned k = 0; k < next_line.size(); k++) {
      if(k == 0) {
        assert(current_line[size] == ' ');
        current_line[size] = next_line[k];
      } else {
        s.push_back(next_line[k]);
      }
    }
    current_line.insert_many(s);

    if((int)(i-start) == numrows()-1) {
      start++;
    }

    buffer.move_right();
    buffer.del();
    buffer.move_left();
    cursor_to(i, size);
  }
  buffer.saved_j = j;
  fix_gap();
}

static void put_tab() {
  auto &j = buffer.cursor.j;
  for(char i = 0; i < tabstop; i++) {
    buffer[buffer.cursor.i].add(' ');
    j++;
  }
}
#endif

#if 0
static void open_console() {
  Mode = Console;
}

static void close_console() {
  Mode = Editor;
  buffer.console.clear();
}
#endif

#if 0
static void put_character_console(const int key) {
  auto shifted = slice(key_lookup, untouchable);
  auto &console = buffer.console;

  // @Note: Need to load settings from 
  // file and generate keyscodes.
  for_each(key_lookup) {
    if(key == *it) {
      char push;
      if(is_shift && isalpha(key)) {
        push = toupper(key);

      } else if(is_shift && in(shifted, key)) {
        const SDL_Keycode *sh = it;
        sh += key_offset;
        push = (char)*sh;

      } else {
        push = (char)key;
      }

      console.add(push);
      return;
    }
  }
}

static void put_character_editor(const int key) {
  // Copy&Paste.
  auto shifted = slice(key_lookup, untouchable);

  // @Note: Need to load settings from 
  // file and generate keyscodes.
  for_each(key_lookup) {
    if(key == *it) {
      char push;
      if(is_shift && isalpha(key)) {
        push = toupper(key);

      } else if(is_shift && in(shifted, key)) {
        const SDL_Keycode *sh = it;
        sh += key_offset;
        push = (char)*sh;

      } else {
        push = (char)key;
      }

      buffer[buffer.cursor.i].add(push);
      cursor_right_no_move();
      return;
    }
  }
}

void handle_editor_keydown(const SDL_Event &e) {
  auto keysym = e.key.keysym;
  auto key = keysym.sym;
  auto mod = keysym.mod;
  is_shift = (mod == KMOD_SHIFT); 
  is_ctrl  = (mod == KMOD_CTRL);

  if(key == SDLK_LSHIFT || key == SDLK_RSHIFT) {
    SDL_SetModState((SDL_Keymod)KMOD_SHIFT);
    return;

  } else if(key == SDLK_LCTRL) {
    SDL_SetModState((SDL_Keymod)KMOD_CTRL);
    return;

  } else if(key == SDLK_ESCAPE) {
    should_quit = true;
    return;

  } else if(key == SDLK_TAB) {
    put_tab();
    return;

  } else if(key == SDLK_BACKSPACE) {
    backspace_key();
    return;

  } else if(key == SDLK_DELETE) {
    delete_key();
    return;

  } else if(key == SDLK_RETURN) {
    return_key();
    return;

  } else if(key == SDLK_UP) {
    cursor_up();
    return;

  } else if(key == SDLK_DOWN) {
    cursor_down();
    return;

  } else if(key == SDLK_LEFT) {
    cursor_left();
    return;

  } else if(key == SDLK_RIGHT) {
    cursor_right();
    return;

  } else if(key == SDLK_r && is_ctrl) {
    open_console();
    return;
  }
  put_character_editor(key);
}

void exec_command() {
  char *c = to_c_string(buffer.console);
  exec_command(c);
  free(c);
}

void handle_console_keydown(const SDL_Event &e) {
  auto keysym = e.key.keysym;
  auto key = keysym.sym;
  is_shift = (keysym.mod == KMOD_SHIFT);

  if(key == SDLK_LSHIFT || key == SDLK_RSHIFT) {
    SDL_SetModState((SDL_Keymod)KMOD_SHIFT);
    return;

  } else if(key == SDLK_ESCAPE) {
    close_console();
    return;

  } else if(key == SDLK_RETURN) {
    exec_command();
    close_console();
    return;
  }
  put_character_console(key);
}
#endif
