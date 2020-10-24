#include "gamma/pch.h"
#include "gamma/commands.h"
#include "gamma/console.h"
#include "gamma/buffer.h"

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

void hsplit(const literal &l) {
  auto tab = get_current_tab();
  if(!l.data) {
    auto p_buf = get_current_buffer();
    open_existing_buffer(p_buf);
    auto n_buf = get_current_buffer();

    {
      n_buf->filename = p_buf->filename;
      n_buf->init(p_buf->start_x + p_buf->width/2., p_buf->start_y, p_buf->width/2., p_buf->height);
      p_buf->init(p_buf->start_x, p_buf->start_y, p_buf->width/2., p_buf->height);
    }

  } else {
    string_t n_filename = to_string(l);

    auto p_buf = get_current_buffer();
    open_existing_or_new_buffer(to_literal(n_filename));
    auto n_buf = get_current_buffer();

    {
      n_buf->filename = std::move(n_filename);
      n_buf->init(p_buf->start_x + p_buf->width/2., p_buf->start_y, p_buf->width/2., p_buf->height);
      p_buf->init(p_buf->start_x, p_buf->start_y, p_buf->width/2., p_buf->height);
    }
  }
}
