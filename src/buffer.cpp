#include "pch.h"
#include "buffer.h"
#include "init.h"
#include "font.h"
#include "console.h"
#include "interp.h"

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
      lines->add(literal(data, size));
      size_t s = size + split_by.size;
      data   += s;
      count  += s;
      size    = 0;
    } else {
      size++;
      count++;
    }
  }
  lines->add(literal(data, size));
}

static array<tab_t>  tabs;
static tab_t        *active_tab = NULL;

static selection_buffer_t selection;

array<tab_t>      &get_tabs()            { return tabs; }
tab_t             *&get_current_tab()    { return assert(active_tab), active_tab; }
buffer_t          *&get_current_buffer() { return assert(active_tab->current_buffer), active_tab->current_buffer; }
selection_buffer_t &get_selection()      { return selection; }



tab_t    *get_free_tab()    { return tabs.add(); }
buffer_t *get_free_buffer() { return active_tab->buffers.add(); }

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
  auto tab   = get_free_tab();
  active_tab = tab;
  open_new_buffer(s); // @Incomplete: What if `s` is already opened?
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

  buffer->total_lines = buffer->count_total_lines();
  return buffer;
}

void open_existing_buffer(buffer_t *prev) {
  auto buffer = get_free_buffer();
  active_tab->current_buffer = buffer;

  buffer->buffer   = prev->buffer;
  buffer->undo     = prev->undo;
  buffer->redo     = prev->redo;
  buffer->filename = prev->filename;
  buffer->buffer.move_until(0);
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
    Lexer lexer;
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
        array<literal> lines;
        defer { free_array(&lines); };

        split(&lines, l, "\n");

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

          lines.first() = literal(first,lines.first().size+1);
          lines.last()  = literal(last, lines.last().size+1);

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
        array<literal> lines;
        defer { free_array(&lines); };
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
            draw_text_shaded(get_font(), comment, c, background_color, px, py);
          } else {
            static_string_from_literal(comment, lines[i]);
            draw_text_shaded(get_font(), comment, c, background_color, px, py);
          }
        }
        continue;

      } else if(syntax->tokenize_comments && tok.type == TOKEN_MULTI_LINE_COMMENT) {
        array<literal> lines;
        defer { free_array(&lines); };
        split(&lines, l, "\n");

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

int buffer_t::get_line_length(int beginning) const {
  int count = 1;
  for(int i = beginning; buffer[i] != '\n'; i++) {
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
    while(buffer[offset_from_beginning-count] != '\n') {
      if(count == offset_from_beginning) { count++; break; }
      count++;
    }
  }
  count--;

  offset_from_beginning -= count;
  start_pos--;
}

int buffer_t::get_cursor_pos_on_line() const {
  assert(offset_on_line == 0 && buffer[cursor] == '\n');
  if(cursor == 0) return 0;
  
  int count = 1;
  while(buffer[cursor-count] != '\n') {
    if(cursor == count) { return count; }
    count++;
  }
  return count-1;
}

int buffer_t::count_total_lines() const {
  int count = 0;
  for(auto i = 0u; i < buffer.size(); i++) {
    if(buffer[i] == '\n') count++;
  }
  return count;
}

void buffer_t::go_right() {
  if(cursor == buffer.size()-1) return;

  //current_action(this);

  buffer.move_right();
  if(buffer[cursor] == '\n') {
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

  //current_action(this);

  buffer.move_left();

  // dec_cursor.
  cursor--;
  if(buffer[cursor] == '\n') {
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
    buffer.move_right();

    go_left();

    if(buffer[cursor] == '\n') total_lines--;
    buffer.backspace();
  }
}

void buffer_t::put_delete() {
  if(cursor == buffer.size()-1) return;
  if(buffer[cursor] == '\n') total_lines--;

  buffer.del();
}

void buffer_t::put_return() {
  buffer.add('\n');
  total_lines++;

  go_right();
  buffer.move_left(); // Same hack as for ::put_backspace().
  assert(offset_on_line == 0);

  //int indent = get_current_line_indent(this);
  //for(int i = 0; i < indent; i++) { put(' '); }
}

void buffer_t::put(char c) {
  buffer.add(c);

  go_right();
  buffer.move_left();
}

void buffer_t::put_tab() {
  for(int i = 0; i < tabstop; i++) { put(' '); }
}

void buffer_t::go_down() {
  if(n_line == total_lines-1) return;

  const size_t tmp_saved_pos 		  = saved_pos;
  const size_t prev_character_pos = n_character;
  for(size_t i = cursor; buffer[i] != '\n'; i++) {
    go_right();
  }

  go_right();
  assert(n_character == 0 && offset_on_line == 0);
  
  const size_t go_till = max(prev_character_pos, tmp_saved_pos);
  for(size_t i = 0; i < go_till; i++) {
    if(buffer[cursor] == '\n') break;
    go_right();
  }

  saved_pos = tmp_saved_pos;
}

void buffer_t::go_up() {
  if(n_line == 0) return;

  const size_t tmp_saved_pos = saved_pos;
  const size_t prev_character_pos = n_character;
  for(auto i = 0u; i < prev_character_pos; i++) {
    go_left();
  }
  assert(n_character == 0 && offset_on_line == 0 && n_line > 0);
	go_left();

  while(n_character > 0) { // go till the beginning of line.
    go_left();
    if(buffer[cursor] == '\n') break;
  }

  const size_t go_till = max(prev_character_pos, tmp_saved_pos);
  for(size_t i = 0; i < go_till; i++) { // from beginning to actual position.
    if(buffer[cursor] == '\n') break;
    go_right();
  }

  saved_pos = tmp_saved_pos;
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

int number_lines_fits_in_window(const buffer_t *b) {
  return (b->height < font_height) ? 1 : b->height/font_height;
}

int number_chars_on_line_fits_in_window(const buffer_t *b) {
  assert(b->width > font_width);
  return b->width / font_width - 1;
}

/*
int get_current_line_indent(buffer_t *buffer) {
  int n_character = buffer->n_character;
  go_to_beginning_of_line();
  assert(buffer->n_character == 0);

  int indent = 0;
  while(buffer->buffer[buffer->cursor+indent+1] == ' ') { indent++; } // +1 for '\n'

  while(buffer->n_character != n_character) { buffer->go_right(); }

  if(n_character < indent) { return n_character; }
  return indent;
}
*/

void no_action(buffer_t *buffer) {}

void select_action(buffer_t *buffer) {
  auto &selection = get_selection();
  auto &cursor    = buffer->cursor;

  if(cursor > selection.first) {
    selection.last = cursor;
  } else {
    selection.first = cursor;
  }
}

void delete_action(buffer_t *buffer) {
  buffer->put_backspace();
}

void init(int argc, char **argv) {
  init_variable_table();

  console_init(); // console is used for report_error() so we need to initialize it before parsing command line arguments.


  string filename;
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
          report_error("Error: use `--` for command line options please.\n");
          continue;
        }
      } else {
        // It's a positional argument.
        if(!filename.size) filename = string(arg, len);
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
void go_to_beginning_of_line() {
  auto buffer = get_current_buffer();
  while(buffer->n_character > 0) { buffer->go_left(); }
}

void go_to_end_of_line() {
  auto buffer = get_current_buffer();
  auto &gap_buffer = buffer->buffer;
  while(gap_buffer[buffer->cursor] != '\n') { buffer->go_right(); }
}

// @StartEndNotHandled: corner cases just don't handled.
void go_word_forward() { // @StartEndNotHandled: 
  auto buffer = get_current_buffer();
  auto &gap_buffer = buffer->buffer;
  while(gap_buffer[buffer->cursor] == ' ') { buffer->go_right(); }
  while(gap_buffer[buffer->cursor] != ' ') { buffer->go_right(); }
}

void go_word_backwards() { // @StartEndNotHandled: 
  auto buffer = get_current_buffer();
  auto &gap_buffer = buffer->buffer;
  while(gap_buffer[buffer->cursor] == ' ') { buffer->go_left(); }
  while(gap_buffer[buffer->cursor] != ' ') { buffer->go_left(); }
}

void go_paragraph_forward() { // @StartEndNotHandled: 
  auto buffer = get_current_buffer();

  while(buffer->n_character != 0) { buffer->go_right(); }
  while(buffer->get_line_length(buffer->cursor) == 1) {
    assert(buffer->n_character == 0);
    buffer->go_down();
  }

  while(buffer->get_line_length(buffer->cursor) != 1) {
    assert(buffer->n_character == 0);
    buffer->go_down();
  }
}

void go_paragraph_backwards() { // @StartEndNotHandled: 
  auto buffer = get_current_buffer();

  while(buffer->n_character != 0) { buffer->go_left(); }
  while(buffer->get_line_length(buffer->cursor) == 1) {
    assert(buffer->n_character == 0);
    buffer->go_up();
  }

  while(buffer->get_line_length(buffer->cursor) != 1) {
    assert(buffer->n_character == 0);
    buffer->go_up();
  }
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
static bool string_match(const gap_buffer &buffer, size_t starting_index, string s) {
  if(buffer.size()-starting_index < s.size) { return false; }
  for(size_t i = 0; i < s.size; i++) {
    if(buffer[i+starting_index] != s[i]) {
      return false;
    }
  }
  return true;
}

void find_in_buffer(string s) {
  auto buffer = active_tab->current_buffer;
  if(buffer->found.size) { buffer->found.clear(); }

  size_t *go_to = NULL;

  size_t cursor = 0, nline = 0, nchar = 0;
  while(cursor != buffer->buffer.size()) {

    if(string_match(buffer->buffer, cursor, s)) {
      Loc *loc = buffer->found.add();
      loc->index = cursor;
      loc->l     = nline;
      loc->c     = nchar;
      loc->size  = s.size;

      go_to = (loc->index >= buffer->cursor) ? &loc->index : NULL;
    }

    if(buffer->buffer[cursor] == '\n') {
      nline++;
      nchar = 0;
    } else {
      nchar++;
    }
    cursor++;

    if(go_to) {
      while(buffer->cursor != *go_to) {
        buffer->go_right();
      }
      break;
    }
  }
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

