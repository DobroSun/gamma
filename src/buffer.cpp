#include "pch.h"
#include "buffer.h"
#include "init.h"
#include "font.h"
#include "console.h"
#include "interp.h"
#include "input.h"

extern void (*current_action)(buffer_t*);


static literal get_file_extension(string filename) {
  literal r;

  size_t index;
  if(filename.find('.', &index)) {
    r.data = filename.data + index;
    r.size = filename.size - index;
  }
  return r;
}

static void split(array<literal> *lines, literal l, literal split_by) {
  const char *data  = l.data;
  size_t count = 0;
  size_t size  = 0;
  while(count < l.size) {
    if(data + size == split_by) {
      lines->add(to_literal(data, size));
      size_t s = size + split_by.size;
      data   += s;
      count  += s;
      size    = 0;
    } else {
      size++;
      count++;
    }
  }
  lines->add(to_literal(data, size));
}

static array<tab_t>  tabs = {};
static tab_t        *active_tab = NULL;

static Select_Buffer selection = {};
static gap_buffer yielded = {};

array<tab_t>       &get_tabs()           { return tabs; }
tab_t             *&get_current_tab()    { return assert(active_tab), active_tab; }
buffer_t          *&get_current_buffer() { return assert(active_tab->current_buffer), active_tab->current_buffer; }
Select_Buffer      &get_selection()      { return selection; }
buffer_t          * get_free_buffer()    { return active_tab->buffers.add(); }


size_t read_file_into_memory(FILE *f, char **mem, size_t gap_len) {
  size_t size, result;
  fseek(f, 0, SEEK_END);
  size = ftell(f);
  rewind(f);

  *mem   = (char*)malloc(size + gap_len);
  result = fread(*mem + gap_len, sizeof(char), size, f);

  if(result != size) { fprintf(stderr, "@Incomplete:"); }
  return size + gap_len;
}

tab_t *open_new_tab(string s) {
  auto tab   = tabs.add();
  active_tab = tab;
  open_new_buffer(s); // @Incomplete: What if `string` is already opened?
  resize_tab(tab);
  return tab;
}

buffer_t *open_new_buffer(string s) {
  auto buffer = get_free_buffer();
  active_tab->current_buffer = buffer;

  assert(s.size != 0);

  if(FILE *f = fopen(s.data, "r")) {
    defer { fclose(f); };

    // Read into gap_buffer.
    auto gap_buffer = &buffer->buffer;
    size_t n = read_file_into_memory(f, &gap_buffer->chars.data, gap_buffer->gap_len);
    gap_buffer->chars.size     = n;
    gap_buffer->chars.capacity = n;

    buffer->filename = s;
  }

  buffer->total_lines = buffer->count_all_lines();
  return buffer;
}

void open_existing_buffer(buffer_t *prev) {
  auto buffer = get_free_buffer();
  active_tab->current_buffer = buffer;

  buffer->buffer   = prev->buffer;
  buffer->undo     = prev->undo;
  buffer->redo     = prev->redo;
  buffer->filename = prev->filename;
  buffer->move_to(0);
}

void open_existing_or_new_buffer(string s) {
  buffer_t *p = find_if(active_tab->buffers,
                        [s](buffer_t b) { return b.filename == s; });

  if(p) {
    open_existing_buffer(p);
  } else {
    open_new_buffer(s);
  }
}

void change_tab(s32 index) {
  if(index >= 0 && index < tabs.size) {
    active_tab = &tabs[index];
  } else {
    // Nothing.
  }
}

void buffer_t::draw() const {
  const size_t buffer_size = buffer.size();

  size_t i = offset_from_beginning;

  int x = get_relative_pos_x(-offset_on_line); // @CleanUp: 
  int y = get_relative_pos_y(0);

  while(i < buffer.size()) {
    int current_line_length = get_line_length(i);

    char string[current_line_length+1] = {0};

    for(size_t j = 0; j < current_line_length; j++) {
      string[j] = (buffer[j+i] == '\n') ? ' ' : buffer[j+i];
    }

    if(y >= get_console()->bottom_y - font_height) { break; }

    draw_text_shaded(get_font(), string, text_color, background_color, x, y);

    y += font_height;
    i += current_line_length;
  }

  char *string = string_from_gap_buffer(&buffer);
  defer { deallocate(string); };

  auto syntax = get_language_syntax(get_file_extension(filename));
  if(syntax) {
    Lexer lexer = {};
    copy_array(&lexer.keywords_table, &syntax->keywords);
    lexer.tokenize_comments   = syntax->tokenize_comments;
    lexer.single_line_comment = to_literal(syntax->single_line_comment);
    lexer.start_multi_line    = to_literal(syntax->start_multi_line);
    lexer.end_multi_line      = to_literal(syntax->end_multi_line);

    defer {
      free_array(&lexer.tokens);
      free_array(&lexer.keywords_table);
    };

    lexer.process_input(string);

    for(auto &tok : lexer.tokens) {
      if(tok.l < start_pos && tok.type != TOKEN_STRING_LITERAL && tok.type != TOKEN_MULTI_LINE_COMMENT && tok.type != TOKEN_SINGLE_LINE_COMMENT) continue;

      literal l = tok.string_literal;

      if(syntax->defined_color_for_literals && (tok.type == TOKEN_NUMBER || tok.type == TOKEN_BOOLEAN)) {
        const int px = get_relative_pos_x(-offset_on_line + tok.c); // @Hack:
        const int py = get_relative_pos_y(tok.l - start_pos);

        if(py > get_console()->bottom_y - font_height) break;

        static_string_from_literal(s, l);
        draw_text_shaded(get_font(), s, syntax->color_for_literals, background_color, px, py);
        continue;

      } else if(syntax->defined_color_for_strings && tok.type == TOKEN_STRING_LITERAL) {
        array<literal> lines = {};
        defer { free_array(&lines); };

        split(&lines, l, to_literal("\n"));

        if(lines.size == 1) {
          const int px = get_relative_pos_x(-offset_on_line + tok.c); // @Hack:
          const int py = get_relative_pos_y(tok.l - start_pos);

          if(py > get_console()->bottom_y - font_height) break;

          SDL_Color c = syntax->color_for_strings;
          static_string_from_literal(s, lines[0]);
          if(lines[0].size) draw_text_shaded(get_font(), s, c, background_color, px + font_width, py);

          char cc = '\"';
          draw_text_shaded(get_font(), cc, c, background_color, px, py);
          draw_text_shaded(get_font(), cc, c, background_color, px + (l.size+1)*font_width, py);
        } else {

          char first[lines.first().size+1];
          first[0] = '\"';
          memcpy(first+1, lines.first().data, lines.first().size);

          char last[lines.last().size+1];
          last[lines.last().size] = '\"';
          memcpy(last, lines.last().data, lines.last().size);

          lines.first() = to_literal(first,lines.first().size+1);
          lines.last()  = to_literal(last, lines.last().size+1);

          for(size_t i = 0; i < lines.size; i++) { // @Copy&Paste: from TOKEN_SINGLE_LINE_COMMENT.
            if(lines[i].size == 0) { continue; }

            int x = (i == 0) ? tok.c : 0;
            const int px = get_relative_pos_x(-offset_on_line + x); // @Hack:
            const int py = get_relative_pos_y(tok.l - start_pos + i);

            if(py > get_console()->bottom_y - font_height) break;
        
            static_string_from_literal(comment, lines[i]);
            draw_text_shaded(get_font(), comment, syntax->color_for_strings, background_color, px, py);
          }
        }
        continue;

      } else if(syntax->tokenize_comments && tok.type == TOKEN_SINGLE_LINE_COMMENT) {
        array<literal> lines = {};
        defer { free_array(&lines); };
        split(&lines, l, to_literal("\\\n"));

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
            draw_text_shaded(get_font(), comment, c, background_color, px, py);
          } else {
            static_string_from_literal(comment, lines[i]);
            draw_text_shaded(get_font(), comment, c, background_color, px, py);
          }
        }
        continue;

      } else if(syntax->tokenize_comments && tok.type == TOKEN_MULTI_LINE_COMMENT) {
        array<literal> lines = {};
        defer { free_array(&lines); };
        split(&lines, l, to_literal("\n"));

        for(size_t i = 0; i < lines.size; i++) { // @Copy&Paste: from TOKEN_SINGLE_LINE_COMMENT.
          if(lines[i].size == 0) { continue; }

          int x = (i == 0) ? tok.c : 0;
          const int px = get_relative_pos_x(-offset_on_line + x); // @Hack:
          const int py = get_relative_pos_y(tok.l - start_pos + i);

          if(py > get_console()->bottom_y - font_height) break;
      
          static_string_from_literal(comment, lines[i]);
          draw_text_shaded(get_font(), comment, syntax->color_for_comments, background_color, px, py);
        }
        continue;
      }

      const struct string *it; size_t index;
      syntax->names.find(l, &it, &index);
      if(it) {
        const int px = get_relative_pos_x(-offset_on_line + tok.c); // @Hack:
        const int py = get_relative_pos_y(tok.l - start_pos);

        if(py > get_console()->bottom_y - font_height) break;
        
        draw_text_shaded(get_font(), it->data, syntax->colors[index], background_color, px, py);
      }
    }
  }

  // search highlighting.
  for(size_t i = 0; i < found.size; i++) {
    Loc loc = found[i];

    if(loc.l < start_pos) continue;

    char s[loc.size+1];
    memcpy(s, string + loc.index, loc.size);
    s[loc.size] = '\0';

    const int px = get_relative_pos_x(-offset_on_line + loc.c); // @Hack:
    const int py = get_relative_pos_y(loc.l - start_pos);

    draw_text_shaded(get_font(), s, searched_text_color, searched_color, px, py);
  }
  // 
}

int buffer_t::get_relative_pos_x(int n_place) const { return start_x + font_width * n_place; }
int buffer_t::get_relative_pos_y(int n_place) const { return start_y + font_height * n_place; }

size_t buffer_t::cursor()        const { return buffer.pre_len; }
char buffer_t::getchar(size_t i) const { return buffer[i]; }
bool buffer_t::start(size_t i)   const { return i == 0; }
bool buffer_t::eol(size_t i)     const { return getchar(i) == '\n'; }
bool buffer_t::eof(size_t i)     const { return i == buffer.size()-1; }

char buffer_t::getchar() const { return getchar(cursor()); }
bool buffer_t::start()   const { return cursor() == 0; }
bool buffer_t::eol()     const { return eol(cursor()); }
bool buffer_t::eof()     const { return eof(cursor()); }


size_t buffer_t::get_line_length(size_t cursor) const {
  size_t count = cursor;
  while(1) {
    if(getchar(count) == '\n' || eof(count)) { break; }
    count++;
  }
  return count-cursor+1;
}

size_t buffer_t::count_all_lines() const {
  size_t count = 0;
  while(1) {
    if(eof(count)) { break; }
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
  assert(offset_from_beginning > 0);

  int count = 2;
  if(offset_from_beginning < count) {
    assert(offset_from_beginning == 1);
  } else {
    while(buffer[offset_from_beginning-count] != '\n') {
      if(count == offset_from_beginning) { count++; break; }
      count++;
    }
  }
  count--;

  offset_from_beginning -= count;
  start_pos--;
}

void select_to_left(buffer_t*);
void select_to_right(buffer_t*);
void delete_to_left(buffer_t*);
void delete_to_right(buffer_t*);

size_t buffer_t::to_left(size_t cursor) {
  if(start(cursor)) return cursor;

  cursor--;
  if(eol(cursor)) {
    if(start_pos == n_line && n_line != 0) { shift_beginning_down(); }

    size_t tmp = cursor;
    while(!start(tmp) && !eol(tmp-1)) { tmp--; }
    n_character = get_line_length(tmp)-1;
    n_line--;
  } else {
    n_character--;
  }
  return cursor;
}

size_t buffer_t::to_right(size_t cursor) {
  if(eof(cursor)) return cursor;
  if(eol(cursor)) {
    if(number_lines_fits_in_window(this)+start_pos-1 == n_line) { shift_beginning_up(); }
    n_character = 0;
    n_line++;
  } else {
    n_character++;
  }
  cursor++;
  return cursor;
}

size_t buffer_t::to_down(size_t cursor) {
  if(n_line == total_lines-1) return cursor;

  size_t n_prev = n_character;
  cursor = to_end_of_line(cursor);
  cursor = to_right(cursor);

  size_t index = min(get_line_length(cursor)-1, n_prev);
  for(size_t i = 0; i < index; i++) {
    cursor = to_right(cursor);
  }
  return cursor;
}

size_t buffer_t::to_up(size_t cursor) {
  if(n_line == 0) return cursor;

  size_t n_prev = n_character;
  size_t size;

  cursor = to_beginning_of_line(cursor);
  cursor = to_left(cursor);

  if(n_character > n_prev) {
    size_t size = n_character;
    for(size_t i = 0; i < size-n_prev; i++) { cursor = to_left(cursor); }
  }
  return cursor;
}

void buffer_t::move_to(size_t i) {
#if 0
  size_t first = min(cursor(), i);
  size_t last  = max(cursor(), i);

  while(i < cursor()) { buffer.move_left(); }
  assert(i >= cursor());

  if(current_action == select_action) {
    selection.first = min(selection.first, first);
  }

  print(selection.first);

  assert(last >= first);
  size_t diff = last - first;
  while(diff--) {
    //if(current_action == 
    current_action(this);
    buffer.move_right();

    print("Selection: ", selection.last);
    

    //if(current_action == delete_action) {
      //current++;
    //} else {
    //}
  }

  while(i < cursor()) { buffer.move_left(); }
  current_action = (current_action == delete_action) ? no_action : current_action;
#endif

//#if 0
  size_t cursor = this->cursor();
  void (*select)(buffer_t*);
  void (*delete_)(buffer_t*);

  while(i != cursor) {

    // 
    // @Note: This turns out to be we need two overloads (left/right) for every action presented, 
    // because right/left directions are not symmetric, so we get weird behaviour, without this trick.
    // I haven't found a way around, so for now, it's going to be like this.
    // 
    if(i < cursor) {
      buffer.move_left();
      select  = select_to_left;
      delete_ = delete_to_left;
    } else {
      buffer.move_right();
      select  = select_to_right;
      delete_ = delete_to_right;
    }


    if(current_action == select_action) {
      current_action = select;
    }
    if(current_action == delete_action) {
      current_action = delete_;
    }
    current_action(this);
    if(current_action == select && selection.first == selection.last) {
      current_action = select_action;
    }

    if(current_action == delete_to_right) {
      cursor++;
    } else {
      cursor = this->cursor();
    }
  }

  if(current_action == delete_to_right || current_action == delete_to_left) {
    current_action = no_action;
  }
//#endif
}

size_t buffer_t::to_beginning_of_line(size_t cursor)  {
  if(cursor > 0 && getchar() == '\n' && getchar(cursor-1) == '\n') { return cursor; } // empty line.
  if(cursor == 0) return cursor;

  while(1) {
    cursor = to_left(cursor);
    if(eol(cursor))   { return to_right(cursor); }
    if(start(cursor)) { return cursor; }
  }
  assert(0);
  return 0;
}

size_t buffer_t::to_end_of_line(size_t cursor) {
  while(!eol(cursor) && !eof(cursor)) { cursor = to_right(cursor); }
  return cursor;
}

void buffer_t::put_backspace() {
  if(start()) return;

  to_left(cursor());

  buffer.move_left();
  put_delete();
}

void buffer_t::put_delete() {
  if(eof()) return;
  if(eol()) total_lines--;
  buffer.del();
}

void buffer_t::put_return() {
  buffer.add('\n');
  total_lines++;

  for(size_t i = 0; i < indentation_level; i++) { put(' '); }

  n_character = indentation_level;
  n_line++;
}

void buffer_t::put(char c) {
  if(c != '\n') {
    buffer.add(c);
    n_character++;
  } else {
    put_return();
  }
}

void buffer_t::put_tab() {
  indentation_level += tabstop;
  for(int i = 0; i < tabstop; i++) { put(' '); }
}

void buffer_t::scroll_down() {
  if(start_pos == total_lines-1) return;
  if(start_pos == n_line) { go_down(); } // @FixMe: 
  shift_beginning_up();
}

void buffer_t::scroll_up() {
  if(offset_from_beginning == 0) return;

  if(number_lines_fits_in_window(this)+start_pos-1 == n_line) {
    go_up(); // @FixMe: 
  }
  shift_beginning_down();
}

void buffer_t::go_left()  { move_to(to_left(cursor())); }
void buffer_t::go_right() { move_to(to_right(cursor())); }
void buffer_t::go_down()  { move_to(to_down(cursor())); }
void buffer_t::go_up()    { move_to(to_up(cursor())); }

void buffer_t::go_to(size_t i) {
  while(i != cursor()) {
    if(i < cursor()) {
      go_left();
    } else {
      go_right();
    }
  }
}

int number_lines_fits_in_window(const buffer_t *b)         { return (b->height < font_height) ? 1 : b->height/font_height; }
int number_chars_on_line_fits_in_window(const buffer_t *b) { assert(b->width > font_width); return b->width / font_width - 1; }

void delete_selected(buffer_t *buffer) {
  yield_selected(buffer);

  current_action = no_action;
  buffer->go_to(selection.first);

  for(size_t i = buffer->cursor(); i <= selection.last; i++) { buffer->put_delete(); }
}

void yield_selected(buffer_t *buffer) {
  yielded.clear();
  for(size_t i = selection.first; i <= selection.last; i++) {
    yielded.add(buffer->buffer[i]);
  }
}

void paste_from_buffer(buffer_t *buffer) {
  for(size_t i = 0; i < yielded.size(); i++) {
    buffer->put(yielded[i]);
  }
}



void no_action(buffer_t*) {}
void select_action(buffer_t *buffer) { /*select_to_right(buffer);*/ }
void delete_action(buffer_t *buffer) { /*delete_to_left(buffer);*/ }


void yield_action(buffer_t *buffer) {
  yielded.add(buffer->getchar());
}

void select_to_right(buffer_t *buffer) { selection.last = buffer->cursor(); }
void select_to_left(buffer_t *buffer)  { selection.first = buffer->cursor(); }
void delete_to_right(buffer_t *buffer) { buffer->put_backspace(); }
void delete_to_left(buffer_t *buffer)  { buffer->put_delete(); }


void update_indentation_level(buffer_t *buffer) {
  size_t indentation_level = 0;
  size_t start_position = buffer->cursor() - buffer->n_character;
  while(buffer->buffer[start_position++] == ' ') {
    indentation_level++;
  }
  buffer->indentation_level = indentation_level;
}


void init(int argc, char **argv) {
  init_variable_table();

  string filename = {};
  if(argc > 1) {
    for(int i = 1; i < argc; i++) { // parsing command line arguments.
      const char *arg = argv[i];
      int len    = strlen(arg);
      int cursor = 0;

      literal option;
      if(arg[cursor] == '-') {
        cursor++;
        if(arg[cursor] == '-') {
          // It's a command line option.
          cursor++;
          const char *tmp = arg + cursor;
          while(arg[cursor] != '\0' && arg[cursor] != '=') {
            cursor++;
          }
          option.data = tmp;
          option.size = cursor - 2; // `--`.
        } else {
          report_error("Error: use `--` for command line options.\n");
          continue;
        }
      } else {
        // It's a positional argument.
        if(!filename.size) filename = to_string(arg, len);
      }

      if(option == "settings") {
        if(arg[cursor++] != '=') {
          report_error("Error: `settings` option expects a path to settings file.\n", settings_filename);
          continue;
        }
        if(arg[cursor] == '\0') {
          report_error("Error: `settings` option requires non null path.\n");
          continue;
        }
        settings_filename = arg + cursor;
        // done.

      } else { // other options.
      }
    }
  } else {
    // No command line arguments provided.
  }

  {
    char *string;
    defer { free(string); };
    {
      FILE *f = fopen(settings_filename, "r");
      defer { fclose(f); };
      read_file_into_memory(f, &string);
    }
    interp(string);
  }

  update_variables();
  make_font();

  open_new_tab(filename);
}

void save() {
  auto   buffer   = active_tab->current_buffer;
  string filename = buffer->filename;
  if(!filename.size) {
    console_put_text("Error: File has no name.\n");
		return;
	}

  FILE *f = fopen(filename.data, "w");
  if(!f) {
    open(filename.data, O_RDWR | O_CREAT, 0);
    f = fopen(filename.data, "w");
  }
	defer { if(f) fclose(f); };

	size_t i = 0;
	for( ; i < buffer->buffer.size(); i++) {
		fprintf(f, "%c", buffer->buffer[i]);
	}
	if(!buffer->buffer.size() || buffer->buffer[i-1] != '\n') {
		fprintf(f, "\n");
	}

	fflush(f);
	console_put_text("File saved.");
}

// Commands.

// @StartEndNotHandled: corner cases just don't handled.
void go_word_forward() { // @StartEndNotHandled: 
  auto   buffer = get_current_buffer();
  size_t cursor = buffer->cursor();

  while(buffer->getchar(cursor) == ' ') { cursor = buffer->to_right(cursor); }
  while(buffer->getchar(cursor) != ' ') { cursor = buffer->to_right(cursor); }
  buffer->move_to(cursor);
}

void go_word_backwards() { // @StartEndNotHandled: 
  auto   buffer = get_current_buffer();
  size_t cursor = buffer->cursor();

  while(buffer->getchar(cursor) == ' ') { cursor = buffer->to_left(cursor); }
  while(buffer->getchar(cursor) != ' ') { cursor = buffer->to_left(cursor); }
  buffer->move_to(cursor);
}

void go_paragraph_forward() { // @StartEndNotHandled: 
  auto   buffer = get_current_buffer();
  size_t cursor = buffer->cursor();

  while(buffer->n_character != 0) { cursor = buffer->to_right(cursor); }
  while(buffer->get_line_length(cursor) == 1) {
    assert(buffer->n_character == 0);
    cursor = buffer->to_down(cursor);
  }
  while(buffer->get_line_length(cursor) != 1) {
    assert(buffer->n_character == 0);
    cursor = buffer->to_down(cursor);
  }
  buffer->move_to(cursor);
}

void go_paragraph_backwards() { // @StartEndNotHandled: 
  auto   buffer = get_current_buffer();
  size_t cursor = buffer->cursor();

  while(buffer->n_character != 0) { cursor = buffer->to_left(cursor); }
  while(buffer->get_line_length(cursor) == 1) {
    assert(buffer->n_character == 0);
    cursor = buffer->to_up(cursor);
  }
  while(buffer->get_line_length(cursor) != 1) {
    assert(buffer->n_character == 0);
    cursor = buffer->to_up(cursor);
  }
  buffer->move_to(cursor);
}
// 


// Splits.
static void assert_buffers_sorted_by_x(array<buffer_t> buffers) {
  for(size_t i = 0; i < buffers.size-1; i++) {
    assert(buffers[i].start_x <  buffers[i+1].start_x);
    assert(buffers[i].start_y == buffers[i+1].start_y);
  }
}

void change_buffer(s32 left) {
  auto tab     = active_tab;
  auto buffers = tab->buffers;

  assert_buffers_sorted_by_x(buffers);

  size_t index;
  buffers.find_pointer(tab->current_buffer, &index);

  if(left) {
    index = (index) ? index-1 : 0;
  } else {
    index = (index != buffers.size-1) ? index+1 : index;
  }
  tab->current_buffer = &buffers[index];
}

void resize_tab(tab_t *tab) {
  const size_t size  = tab->buffers.size;
  const f32    scale = Width / (f32)size;

  console_on_resize(Height);
  const int h = get_console()->bottom_y;

  for(size_t i = 0; i < size; i++) {
    auto buffer = &tab->buffers[i];

    buffer->start_x = i * scale;
    buffer->start_y = buffer->start_y;
    buffer->width   = scale;
    buffer->height  = h;
  }
}

void close_buffer(tab_t *tab) {
  array<buffer_t> &buffers = tab->buffers;

  size_t index;
  buffers.find_pointer(tab->current_buffer, &index);
  buffers.remove(index);

  if(buffers.size) {
    index = (index == buffers.size) ? index-1 : index;
    tab->current_buffer = &buffers[index];
    resize_tab(tab);

  } else {
    should_quit = true;
  }
}
// 

// Search.
static bool string_match(const gap_buffer &buffer, size_t starting_index, const string s) {
  if(buffer.size()-starting_index < s.size) { return false; }
  for(size_t i = 0; i < s.size; i++) {
    if(buffer[i+starting_index] != s[i]) {
      return false;
    }
  }
  return true;
}


void to_prev_in_search() {
  auto buffer = get_current_buffer();
  if(buffer->found_in_a_file) {
    buffer->search_index = (buffer->search_index) ? buffer->search_index : buffer->found.size;
    Loc loc = buffer->found[--buffer->search_index];
    //go_to
  }
}

void to_next_in_search() {
  auto buffer = get_current_buffer();
  if(buffer->found_in_a_file) {
    Loc loc = buffer->found[++buffer->search_index];
    while(buffer->cursor() != loc.index) {
      buffer->go_right(); // @FixMe: 
    }
  }
}

void find_in_buffer(const string s) {
  auto buffer = get_current_buffer();
  if(buffer->found.size) { buffer->found.clear(); }

  bool found_in_a_file = false;

  size_t cursor = 0;
  size_t nline = 0;
  size_t nchar = 0;

  while(cursor != buffer->buffer.size()) {
    if(string_match(buffer->buffer, cursor, s)) {
      found_in_a_file = true;

      Loc *loc = buffer->found.add();
      loc->index = cursor;
      loc->l     = nline;
      loc->c     = nchar;
      loc->size  = s.size;

    }

    if(buffer->buffer[cursor] == '\n') {
      nline++;
      nchar = 0;
    } else {
      nchar++;
    }
    cursor++;
  }

  buffer->found_in_a_file = found_in_a_file;
  buffer->search_index    = 0;
  
  // @Hack: Well, i don't want to Copy&Paste here, so ...
  to_next_in_search();
  to_prev_in_search();
}
// 

// Undo/Redo.
static void to_previous_buffer_state(buffer_t *b, array<buffer_t> *active_states, array<buffer_t> *backup_states) {
  if(!active_states->size) { return; }
  auto state = active_states->pop();

  { // save current state for backup. @Copy&Paste: of save_current_state_for_undo.
    auto a = backup_states->add(*b);
    copy_gap_buffer(&a->buffer, &b->buffer);
  }

  // We wan't to copy everything, execept for undo, redo arrays.
  const array<buffer_t> undo = b->undo;
  const array<buffer_t> redo = b->redo;

  *b = *state;
  copy_gap_buffer(&b->buffer, &state->buffer);
  b->undo = undo;
  b->redo = redo;
}

void save_current_state_for_undo(buffer_t *b) {
  auto a = b->undo.add(*b);
  copy_gap_buffer(&a->buffer, &b->buffer);
}

void undo(buffer_t *b) { to_previous_buffer_state(b, &b->undo, &b->redo); }
void redo(buffer_t *b) { to_previous_buffer_state(b, &b->redo, &b->undo); }
// 

