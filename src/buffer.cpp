#include "pch.h"
#include "buffer.h"
#include "init.h"
#include "font.h"
#include "console.h"
#include "interp.h"


literal get_file_extension(const string &filename) {
  const char *iter; size_t index;
  filename.find('.', &iter, &index);

  if(iter) {
    literal r;
    r.data = filename.data + index;
    r.size = filename.size - index;
    return r;
  } else {
    literal r;
    return r;
  }
}

void split(array<literal> *lines, const literal &l, const literal &split_by) {
  const char *data = l.data;
  size_t      count = 0;
  size_t      size = 0;
  while(count < l.size) {
    if(l.data + count == split_by) {
      lines->add(literal(data, size));
      size = 0;
      count += split_by.size;
      data = l.data + count;
    } else {
      size++;
      count++;
    }
  }
  lines->add(literal(data, size));
}

// @Speed:
// No need to iterate and get them 
// each time, better solution is to keep 
// an array of currently used bufs.
#define get_used_buffers(name, size_name, buffers) \
  buffer_t *name[buffers.size]; \
  size_t size_name = 0; \
  for(size_t i = 0; i < buffers.size; i++) { \
    if(buffers[i].is_used) { \
      name[size_name++] = &buffers[i]; \
    } \
  }

#define get_const_buffers(name, size_name, buffers) \
  const buffer_t *name[buffers.size]; \
  size_t size_name = 0; \
  for(size_t i = 0; i < buffers.size; i++) { \
    if(buffers[i].is_used) { \
      name[size_name++] = &buffers[i]; \
    } \
  }


static tab_t    *active_tab    = NULL;
static buffer_t *active_buffer = NULL;

static selection_buffer_t selection;

static array<tab_t> tabs;
static file_buffer_t global_copy_buffer;





template<class T>
T *get_free_source(array<T> &source) {
  T *ret = NULL;
  for(size_t i = 0; i < source.size; i++) {
    if(!source[i].is_used) {
      ret = &source[i];
      break;
    }
  }

  if(!ret) {
    ret = &source.add();
    new (ret) T();

  } else {
    // Already found.
  }

  assert(ret);
  ret->is_used = true;
  return ret;
}

static tab_t *get_free_tab()       { return get_free_source(tabs); }
static buffer_t *get_free_buffer() { return get_free_source(active_tab->buffers); }


static void finish_file(file_buffer_t *f) {
  for(size_t i = 0; i < f->undo.size; i++) {
    assert(f->undo[i].file && !f->undo[i].file->undo.size);
    delete f->undo[i].file;
  }
  for(size_t i = 0; i < f->redo.size; i++) {
    assert(f->redo[i].file && !f->redo[i].file->redo.size);
    delete f->redo[i].file;
  }
  delete f;
}

static void finish_buffer(buffer_t *b) {
  b->file    = NULL;
  b->is_used = false;
  b->filename.clear();

  b->start_x = 0; b->start_y = 0; b->width = 0; b->height = 0;
  b->cursor = 0; b->n_character = 0; b->n_line = 0; 
  b->offset_on_line = 0; b->offset_from_beginning = 0;
  b->start_pos = 0; b->saved_pos = 0;
}

static void finish_buffer_and_file(buffer_t *b) {
  finish_file(b->file);
  finish_buffer(b);
}


array<tab_t> &get_tabs() {
  return tabs;
}

tab_t *get_current_tab() {
  return active_tab;
}

buffer_t *get_current_buffer() {
  return active_buffer;
}

selection_buffer_t &get_selection() {
  return selection;
}

// @CleanUp: factor read_entire_file's together.
void read_entire_file(string *s, FILE *f) {
  fseek(f, 0, SEEK_END);
  size_t size = ftell(f);

  s->resize(size);
  rewind(f);

  size_t res = fread(s->data, sizeof(char), size, f);
  if(res != size) { fprintf(stderr, "@Incomplete\n"); }
}


static void read_entire_file(gap_buffer *ret, FILE *f) {
  assert(ret && f);
  fseek(f, 0, SEEK_END);
  size_t size = ftell(f);

  ret->chars.resize(size + ret->gap_len);
  assert(ret->pre_len == 0);

  rewind(f);
  size_t res = fread(ret->chars.data + ret->gap_len, sizeof(char), size, f);

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


void open_new_buffer(string &s) {
  auto buffer = get_free_buffer();
  buffer->file = new file_buffer_t;

  if(s.data == NULL) { return; }

  if(FILE *f = fopen(s.data, "r")) {
    defer { fclose(f); };
    read_entire_file(&buffer->file->buffer, f);
    buffer->filename = s;
  }

  buffer->total_lines = buffer->count_total_lines();
  active_buffer = buffer;
}

void open_existing_buffer(buffer_t *prev) {
  auto buffer = get_free_buffer();

  buffer->file = prev->file;
  buffer->filename = prev->filename;
  buffer->file->buffer.move_until(0);

  active_buffer = buffer;
}

void open_existing_or_new_buffer(const literal &filename) {
  auto tab = get_current_tab();
  get_used_buffers(used_bufs, size, tab->buffers);

  for(size_t i = 0; i < size; i++) {
    if(used_bufs[i]->filename == filename) {
      open_existing_buffer(used_bufs[i]);
      return;
    }
  }
  string s = to_string(filename);
  open_new_buffer(s);
}

void tab_t::draw(bool selection_mode) const {
  get_const_buffers(used_bufs, used_size, buffers);




  for(size_t i = 0; i < used_size; i++) {
    used_bufs[i]->draw(selection_mode);
  }

#if 0
  const buffer_t *same_buffers[used_size];
  size_t size = 0;
  for(size_t i = 0; i < used_size; i++) {
    auto *buffer = used_bufs[i];
    if(buffer->filename == active_buffer->filename &&
       active_buffer->n_line >= buffer->start_pos  &&
       active_buffer->n_line <  buffer->start_pos + number_lines_fits_in_window(buffer) &&
       buffer->file->buffer.is_initialized()) {

      same_buffers[size++] = buffer;
    }
  }

  // Updating only same buffers, as active_buffer.
  for(size_t i = 0; i < size; i++) {
    auto *buffer = same_buffers[i];
    draw_rect(buffer->start_x, buffer->start_y, buffer->width, buffer->height, WhiteColor);
    buffer->draw(selection_mode);
  }
#endif

  if(used_size == 0) {

  } else {
    // Update cursor.
    char s = active_buffer->file->buffer[active_buffer->cursor];
    s = (s == '\n')? ' ': s;

    int px = active_buffer->get_relative_pos_x(active_buffer->n_character-active_buffer->offset_on_line);
    int py = active_buffer->get_relative_pos_y(active_buffer->n_line-active_buffer->start_pos);

    draw_text_shaded(get_font(), s, WhiteColor, BlackColor, px, py);

    draw_rect(0, get_console()->bottom_y, Width, font_height, WhiteColor);
    console_draw();
  }
}

void tab_t::on_resize(int n_width, int n_height) {
  get_used_buffers(used_bufs, size, buffers);

  console_on_resize(n_height);

  for(auto i = 0u; i < size; i++) {
    used_bufs[i]->on_resize(used_bufs[i]->width, used_bufs[i]->height, n_width, get_console()->bottom_y);
  }
}


void buffer_t::draw(bool selecting) const {
  const gap_buffer &buffer      = active_buffer->file->buffer;
  const size_t      buffer_size = buffer.size();

  size_t current_token_index = 0;

  size_t i = offset_from_beginning, line_number = 0;
  while(i < file->buffer.size()) {
    const int current_line_length = get_line_length(i);

    char string[current_line_length + 1];
    string[current_line_length] = '\0';

    for(size_t j = i; j < i + current_line_length; j++) {
      char c = buffer[j];
      if(c == '\n') {
        string[j-i] = ' ';
      } else {
        string[j-i] = c;
      }
    }

    const int px = get_relative_pos_x(-offset_on_line); // @Hack:
    const int py = get_relative_pos_y(line_number);

    if(py >= get_console()->bottom_y - font_height) { break; }

    draw_text_shaded(get_font(), string, BlackColor, WhiteColor, px, py);

    i += current_line_length;
    line_number++;
  }


  auto syntax = get_language_syntax(get_file_extension(filename));
  if(!syntax) return;

  Lexer lexer;
  copy_array(lexer.keywords_table, syntax->keywords);
  lexer.tokenize_comments   = syntax->tokenize_comments;
  lexer.single_line_comment = to_literal(syntax->single_line_comment);
  lexer.start_multi_line    = to_literal(syntax->start_multi_line);
  lexer.end_multi_line      = to_literal(syntax->end_multi_line);

  char string[buffer_size+1];

  memcpy(string, buffer.chars.data, buffer.pre_len);
  memcpy(string + buffer.pre_len, buffer.chars.data + buffer.pre_len + buffer.gap_len, buffer_size-buffer.pre_len);
  string[buffer_size] = '\0';

  lexer.process_input(string);

  auto &tokens = lexer.tokens;
  for(auto &tok : tokens) {
    if(tok.l < start_pos && tok.type != TOKEN_STRING_LITERAL && tok.type != TOKEN_MULTI_LINE_COMMENT && tok.type != TOKEN_SINGLE_LINE_COMMENT) continue;

    literal l = tok.string_literal;

    if(syntax->defined_color_for_literals && (tok.type == TOKEN_NUMBER || tok.type == TOKEN_BOOLEAN)) {
      const int px = get_relative_pos_x(-offset_on_line + tok.c); // @Hack:
      const int py = get_relative_pos_y(tok.l - start_pos);

      if(py > get_console()->bottom_y - font_height) break;

      static_string_from_literal(s, l);
      draw_text_shaded(get_font(), s, syntax->color_for_literals, WhiteColor, px, py);
      continue;

    } else if(syntax->defined_color_for_strings && tok.type == TOKEN_STRING_LITERAL) {
      array<literal> lines;
      split(&lines, l, "\n");

      if(lines.size == 1) {
        const int px = get_relative_pos_x(-offset_on_line + tok.c); // @Hack:
        const int py = get_relative_pos_y(tok.l - start_pos);

        if(py > get_console()->bottom_y - font_height) break;

        SDL_Color c = syntax->color_for_strings;
        static_string_from_literal(s, lines[0]);
        if(lines[0].size) draw_text_shaded(get_font(), s, c, WhiteColor, px + font_width, py);

        char cc = '\"';
        draw_text_shaded(get_font(), cc, c, WhiteColor, px, py);
        draw_text_shaded(get_font(), cc, c, WhiteColor, px + (l.size+1)*font_width, py);
      } else {

        char first[lines.first().size+1];
        first[0] = '\"';
        memcpy(first+1, lines.first().data, lines.first().size);

        char last[lines.last().size+1];
        last[lines.last().size] = '\"';
        memcpy(last, lines.last().data, lines.last().size);

        lines.first() = literal(first,lines.first().size+1);
        lines.last()  = literal(last, lines.last().size+1);

        for(size_t i = 0; i < lines.size; i++) { // @Copy&Paste: from TOKEN_SINGLE_LINE_COMMENT.
          if(lines[i].size == 0) { continue; }

          int x = (i == 0) ? tok.c : 0;
          const int px = get_relative_pos_x(-offset_on_line + x); // @Hack:
          const int py = get_relative_pos_y(tok.l - start_pos + i);

          if(py > get_console()->bottom_y - font_height) break;
      
          static_string_from_literal(comment, lines[i]);
          draw_text_shaded(get_font(), comment, syntax->color_for_strings, WhiteColor, px, py);
        }
      }
      continue;

    } else if(syntax->tokenize_comments && tok.type == TOKEN_SINGLE_LINE_COMMENT) {
      array<literal> lines;
      split(&lines, l, "\\\n");

      for(size_t i = 0; i < lines.size; i++) {
        if(lines[i].size == 0) { continue; }

        int x = (i == 0) ? tok.c : 0;
        const int px = get_relative_pos_x(-offset_on_line + x); // @Hack:
        const int py = get_relative_pos_y(tok.l - start_pos + i);

        if(py > get_console()->bottom_y - font_height) break;

        SDL_Color c = syntax->color_for_comments;
    
        if(i != lines.size-1) {
          char comment[lines[i].size+2];
          memcpy(comment, lines[i].data, lines[i].size);
          comment[lines[i].size] = '\\';
          comment[lines[i].size+1] = '\0';
          draw_text_shaded(get_font(), comment, c, WhiteColor, px, py);
        } else {
          static_string_from_literal(comment, lines[i]);
          draw_text_shaded(get_font(), comment, c, WhiteColor, px, py);
        }
      }
      continue;

    } else if(syntax->tokenize_comments && tok.type == TOKEN_MULTI_LINE_COMMENT) {
      array<literal> lines;
      split(&lines, l, "\n");

      for(size_t i = 0; i < lines.size; i++) { // @Copy&Paste: from TOKEN_SINGLE_LINE_COMMENT.
        if(lines[i].size == 0) { continue; }

        int x = (i == 0) ? tok.c : 0;
        const int px = get_relative_pos_x(-offset_on_line + x); // @Hack:
        const int py = get_relative_pos_y(tok.l - start_pos + i);

        if(py > get_console()->bottom_y - font_height) break;
    
        static_string_from_literal(comment, lines[i]);
        draw_text_shaded(get_font(), comment, syntax->color_for_comments, WhiteColor, px, py);
      }
      continue;
    }


    struct string *it; size_t index;
    syntax->names.find(l, &it, &index);
    if (it) {
      const int px = get_relative_pos_x(-offset_on_line + tok.c); // @Hack:
      const int py = get_relative_pos_y(tok.l - start_pos);

      if(py > get_console()->bottom_y - font_height) break;
      
      draw_text_shaded(get_font(), it->data, syntax->colors[index], WhiteColor, px, py);
    }
  }
}

int buffer_t::get_relative_pos_x(int n_place) const { return start_x + font_width * n_place; }
int buffer_t::get_relative_pos_y(int n_place) const { return start_y + font_height * n_place; }


void buffer_t::on_resize(int prev_width, int prev_height, int new_width, int new_height) {
  start_x = new_width * start_x / prev_width;
  start_y = new_height * start_y / prev_height;
  width   = new_width * width / prev_width;
  height  = new_height * height / prev_height;

  if(n_line >= start_pos + number_lines_fits_in_window(this)) {
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

int buffer_t::count_total_lines() const {
  int count = 0;
  for(auto i = 0u; i < file->buffer.size(); i++) {
    if(file->buffer[i] == '\n') count++;
  }
  return count;
}

void buffer_t::go_right() {
  if(cursor == file->buffer.size()-1) return;

  file->buffer.move_right();
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
  saved_pos = n_character;

  if(number_lines_fits_in_window(this) + start_pos == n_line) {  
    shift_beginning_up();
  }
}


void buffer_t::go_left() {
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
  saved_pos = n_character;


  if(n_character < offset_on_line) {
    offset_on_line--;
  }
  //

  if(start_pos-1 == n_line && start_pos != 0) {
    shift_beginning_down();
  }
}

void buffer_t::put_backspace() {
  if(cursor == 0) {
    // Do nothing.
  } else {
    // We don't need to move gap at all,
    // but since go_left calls       gap_buffer.move_left(),
    // we have to compensate it with gap_buffer.move_right().
    file->buffer.move_right();
    go_left();

    if(file->buffer[cursor] == '\n') total_lines--;
    file->buffer.backspace();
  }
}

void buffer_t::put_delete() {
  if(cursor == file->buffer.size()-1) return;
  if(file->buffer[cursor] == '\n') total_lines--;

  file->buffer.del();
}

void buffer_t::put_return() {
  file->buffer.add('\n');
  total_lines++;

  go_right();
  file->buffer.move_left(); // Same hack as for ::put_backspace().
  assert(offset_on_line == 0);
}

void buffer_t::put(char c) {
  file->buffer.add(c);

  go_right();
  file->buffer.move_left();
}

// @Temporary: @RemoveMe:
void buffer_t::go_down() {
  auto buffer = get_current_buffer();
  int n = buffer->compute_go_down();
  while(n) {
    buffer->go_right();
    n--;
  }
}

void buffer_t::go_up()   {
  auto buffer = get_current_buffer();
  int n = buffer->compute_go_up();
  while(n) {
    buffer->go_left();
    n--;
  }
}

int buffer_t::compute_go_down() {
  if(n_line == total_lines-1) return 0;
  int count = 0;

  const size_t prev_character_pos = n_character;
  for(size_t i = cursor; file->buffer[i] != '\n'; i++) {
    count++;
  }
  count++;
  
  for(size_t i = 0; i < prev_character_pos; i++) {
    if(file->buffer[cursor+count] == '\n') return count;
    count++;
  }
  return count;
}

int buffer_t::compute_go_up() {
  if(n_line == 0) return 0;
  int count = 0;

  const size_t prev_character_pos = n_character;
  for(auto i = 0u; i < prev_character_pos; i++) {
    count++;
  }

  count++;
  assert(file->buffer[cursor-count] == '\n');
  count++;

  if(cursor <= count) {
    if(cursor == count && count == 2) count++;

  } else {
    while(file->buffer[cursor-count] != '\n') { // go till the beginning of line.
      if(cursor == count) {
        count++;
        break;
      }
      count++;
    }
  }

  count--;
  for(size_t i = 0; i < prev_character_pos; i++) { // from beginning to actual position.
    if(cursor < count) break;
    if(file->buffer[cursor-count] == '\n') break;
    count--;
  }

  if(cursor == count) count++;
  return count;
}

void buffer_t::scroll_down() {
  if(start_pos == total_lines-1) return;
  if(start_pos == n_line) { go_down(); }
  shift_beginning_up();
}

void buffer_t::scroll_up() {
  if(offset_from_beginning == 0) return;

  if(number_lines_fits_in_window(this)+start_pos-1 == n_line) {
    go_up();
  }
  shift_beginning_down();
}

void buffer_t::init(int x, int y, int w, int h) {
  start_x = x; start_y = y; width = w; height = h;
}

int number_lines_fits_in_window(const buffer_t *b) {
  return (b->height < font_height) ? 1 : b->height/font_height;
}

int number_chars_on_line_fits_in_window(const buffer_t *b) {
  assert(b->width > font_width);
  return b->width / font_width - 1;
}

void init(int argc, char **argv) {
  init_var_table();

  string filename;
  if(argc > 1) {
    filename = argv[1];
  } else {
    // No positional arguments provided.
    // filename.data == nullptr.
  }

  auto tab = get_free_tab();
  active_tab = tab;
  open_new_buffer(filename); // @Incomplete: What if filename is already opened?

  tab->splits.add(active_buffer); 
  tab->insert_to++;

  auto buffer = active_buffer;

  make_font();


  console_init();
  console_on_resize(Height);
  buffer->init(0, 0, Width, get_console()->bottom_y);
}

selection_buffer_t *get_selection_buffer() {
  return &selection;
}

void delete_selected() {
  assert(selection.start_index != -1);
  auto buffer = active_buffer;

  int cursor = buffer->cursor;
  int index  = selection.start_index;

  if(cursor < index) {
    size_t diff = index - cursor;
    for(size_t i = 0; i < diff; i++) {
      buffer->go_right();
    }

  } else if(cursor > index) {
    size_t diff = cursor - index;
    for(size_t i = 0; i < diff; i++) {
      buffer->go_left();
    }
  } else {
    assert(index == cursor);
  }

  for(size_t i = 0; i < selection.size+1; i++) {
    buffer->put_delete();
  }
}

void copy_selected() {
  assert(selection.start_index != -1);
  int start = selection.start_index;

  auto &gap_buffer = active_buffer->file->buffer;
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
  auto buffer = active_buffer;
  auto &gap_buffer = global_copy_buffer.buffer;

  for(size_t i = 0; i < gap_buffer.size(); i++) {
    buffer->put(gap_buffer[i]);
  }
  console_put_text("Pasted!");
}

void go_to_line(int line) {
  auto total_lines = active_buffer->total_lines;

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

  while(line > (int)active_buffer->n_line+1) active_buffer->go_down();
  while(line < (int)active_buffer->n_line+1) active_buffer->go_up();
}


void save() {
  auto buffer = active_buffer;
  if(buffer->filename.empty()) {
    console_put_text("File has no name.");

  } else {
    assert(!buffer->filename.empty());

    FILE *f = get_file_or_create(buffer->filename.data, "w");
    defer { fclose(f); };

    if(!f) {
      // @Incomplete: report error.
      assert(0);
      print("No file for me (:");
    }

    size_t i = 0u;
    for( ; i < buffer->file->buffer.size(); i++) {
      fprintf(f, "%c", buffer->file->buffer[i]);
    }
    if(!buffer->file->buffer.size() || buffer->file->buffer[i-1] != '\n') {
      fprintf(f, "\n");
    }

    fflush(f);
    console_put_text("File saved.");
  }
}




// Splits.
static int compute_start_to_left(int current, buffer_t *b)  { return current - b->start_x; }
static int compute_start_to_right(int current, buffer_t *b) { return b->start_x - current; }
static int compute_start_to_up(int current, buffer_t *b)    { return current - b->start_y; }
static int compute_start_to_down(int current, buffer_t *b)  { return b->start_y - current; }

void change_split(buffer_t *p, direction_t d) {
  assert(d != none); // Doesn't make any sense to call this with none.

  get_used_buffers(used_bufs, usize, active_tab->buffers);
  if(usize == 1) return;

  buffer_t *n;
  // Ex. for d == left : 
  // Need to find out which buffer has `start_x` < than the current_x.
  // Not the lowest value, just nearest to current.
  // Than find buffer with that value, and change current buffer to it.
  //

  {

  int current; int another;
  if(d == left || d == right) {
    current = p->start_x;
    another = p->start_y;
  } else {
    assert(d == up || d == down);
    current = p->start_y;
    another = p->start_x;
  }
  
  int (*compute_buffer_start_position)(int,buffer_t*) = NULL;
  switch(d) {
    case left  : compute_buffer_start_position = &compute_start_to_left; break;
    case right : compute_buffer_start_position = &compute_start_to_right; break;
    case up    : compute_buffer_start_position = &compute_start_to_up; break;
    case down  : compute_buffer_start_position = &compute_start_to_down; break;
    case none  : assert(0); break;
  }

  int starts[usize];
  for(size_t i = 0; i < usize; i++) {
    starts[i] = compute_buffer_start_position(current, used_bufs[i]);
  }

  std::sort(starts, starts+usize); // Maybe I need my implementation of this?
  if(starts[usize-1] == 0) return; // If there is no values after 0.

  int actual_start; int count = 0;
  bool first_found = true;

  for(size_t i = 0; i < usize; i++) {
    if(starts[i] > 0) {
      if(first_found) { actual_start = starts[i]; first_found = false; }
      count++;
    }
  }

  buffer_t *same_bufs[count]; count = 0;
  for(size_t i = 0; i < usize; i++) {
    if(actual_start == compute_buffer_start_position(current, used_bufs[i])) {
      same_bufs[count++] = used_bufs[i];
    }
  }
  
  if(count == 1) {
    n = same_bufs[0];
  } else {
    if(d == up || d == down) {
      std::sort(same_bufs, same_bufs+count, [](const buffer_t *b, const buffer_t *p) {
        return b->start_x < p->start_x;
      });
    } else {
      assert(d == left || d == right);
      std::sort(same_bufs, same_bufs+count, [](const buffer_t *b, const buffer_t *p) {
        return b->start_y < p->start_y;
      });

    }

    bool break_from_loop = false;
    buffer_t *yield;
    first_found = false; 
    for(size_t i = 0; i < count; i++) {
      if(d == up || d == down) {
        if(same_bufs[i]->start_x < another) {
          first_found = true;
          yield = same_bufs[i];

        } else if(same_bufs[i]->start_x > another) {
          if(first_found) {
            n = yield;
            break_from_loop = true;
            break;
          } else {
            assert(0);
          }
        } else {
          assert(another == same_bufs[i]->start_x);
          n = same_bufs[i];
          break_from_loop = true;
          break;
        }

     } else {
      assert(d == left || d == right);
      if(same_bufs[i]->start_y < another) {
        first_found = true;
        yield = same_bufs[i];

      } else if(same_bufs[i]->start_y > another) {
        if(first_found) {
          n = yield;
          break_from_loop = true;
          break;
        } else {
          assert(0);
        }
      } else {
        assert(another == same_bufs[i]->start_y);
        n = same_bufs[i];
        break_from_loop = true;
        break;
      }
     }
    }
    if(break_from_loop) {
    } else {
      n = yield;
    }
  }

  }
  assert(n && n != p);

  auto &splits = active_tab->splits;

  buffer_t **iter; size_t index;
  splits.find(n, &iter, &index);
  assert(iter);

  active_tab->insert_to = index;
  active_buffer = n;
}

void close_split(buffer_t *current) {
/*
  auto &splits = active_tab->splits;

  if(splits.size == 1) { 
    auto tmp = splits.pop();
    assert(current == tmp);

    finish_buffer_and_file(current);

    active_buffer = NULL;
    should_quit   = true; 
    return;
  }

  assert(splits.size > 1);

  size_t index = splits.remove(current);
  splits[0]->init(0, 0, Width, get_console()->bottom_y);

  for(size_t i = 0; i < splits.size-1; i++) {
    auto p = splits[i];
    auto n = splits[i+1];
    do_split(p, n, n->split_type, true);
  }

  if(index == splits.size) {
    active_buffer = splits[index-1];
  } else {
    active_buffer = splits[index];
  }



  get_used_buffers(used_bufs, usize, active_tab->buffers);
  size_t size = 0;
  for(size_t i = 0; i < usize; i++) {
    if(used_bufs[i]->filename == current->filename) {
      size++;
    }
  }

  if(size == 1) {  // Check for buffers opened multiple times.
    finish_buffer_and_file(current);
  } else {
    if(active_buffer->filename == current->filename) { active_buffer->file->buffer.move_until(active_buffer->cursor); }
    finish_buffer(current);
  }
*/
}


void do_split(buffer_t *p, buffer_t *n, split_type_t type, bool resize_only) {
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

  if(!resize_only) {
    active_tab->splits.insert(n, ++active_tab->insert_to);
    n->split_type = type;
  }
}
// end of Splits.

static const char stop_chars[] = { ' ', '(', '{', ')', '}', '#', '\"', '\'', '/', '\\', '.', ';', '\n' };
int go_word_forward() {
  auto buffer = active_buffer;

  int count = 0;
  if(is_one_of(buffer->file->buffer[buffer->cursor+count], stop_chars)) {
    count++;

  } else {
    count++;
    while(!is_one_of(buffer->file->buffer[buffer->cursor+count], stop_chars)) {
      count++;
    }
  }
  if(buffer->file->buffer[buffer->cursor+count] == ' ') { count++; }
  return count;
}

int go_word_backwards() {
  auto buffer = active_buffer;

  int count = 0;
  if(is_one_of(buffer->file->buffer[buffer->cursor-count], stop_chars)) {
    count++;

  } else {
    int c = 0;
    while(!is_one_of(buffer->file->buffer[buffer->cursor-count], stop_chars)) {
      count++;
      c++;
    }

    if(c != 1) { count--; }
  }
  if(buffer->file->buffer[buffer->cursor-count] == ' ') { count++; }
  return count;
}

int compute_to_beginning_of_line() {
  return active_buffer->n_character;
}

int compute_to_end_of_line() {
  assert(active_buffer->file->buffer[active_buffer->cursor-active_buffer->n_character + active_buffer->get_line_length(active_buffer->cursor-active_buffer->n_character) - 1] == '\n');
  return active_buffer->get_line_length(active_buffer->cursor - active_buffer->n_character) - active_buffer->n_character - 1;
}


// Undo/Redo.
static void save_current_state_for_backup(buffer_t *b, array<buffer_t> *states) {
  auto a  = &states->add();
  new (a) buffer_t();

  a->file = new file_buffer_t;

  copy_gap_buffer(a->file->buffer, b->file->buffer);
  copy_window_position(a, b);
}

static void to_previous_buffer_state(buffer_t *b, array<buffer_t> *active_states, array<buffer_t> *passive_states) {
  if(active_states->empty()) { return; }
  auto state = &active_states->pop();

  save_current_state_for_backup(b, passive_states);

  auto tmp = new file_buffer_t;
  tmp->buffer = state->file->buffer;
  tmp->undo   = b->file->undo;
  tmp->redo   = b->file->redo;

  delete b->file;
  b->file = tmp;

  copy_window_position(b, state);
  delete state->file;
}


void save_current_state_for_undo(buffer_t *b) {
  save_current_state_for_backup(b, &b->file->undo);
}

void undo(buffer_t *b) {
  to_previous_buffer_state(b, &b->file->undo, &b->file->redo);
}

void redo(buffer_t *b) {
  to_previous_buffer_state(b, &b->file->redo, &b->file->undo);
}
// end of Undo/Redo.
