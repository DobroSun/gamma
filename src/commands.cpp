#include "gamma/pch.h"
#include "gamma/commands.h"
#include "gamma/console.h"
#include "gamma/buffer.h"

void go_to_line(int line) {
  auto &buffer = get_current_buffer();
  auto total_lines = buffer.get_total_lines();

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

  while(line > (int)buffer.n_line+1) buffer.go_down();
  while(line < (int)buffer.n_line+1) buffer.go_up();
}


void save() {
  auto &buffer = get_current_buffer();
  if(!buffer.filename) {
    console_put_text("File has no name.");

  } else {
    assert(buffer.filename);
    FILE *f = get_file_or_create(buffer.filename, "w");
    defer { fclose(f); };

    if(!f) {
      // @Incomplete: report error.
      print("No file for me (:");
    }

    unsigned i = 0u;
    for( ; i < buffer.buffer.size(); i++) {
      fprintf(f, "%c", buffer.buffer[i]);
    }
    if(!buffer.buffer.size() || buffer.buffer[i-1] != '\n') {
      buffer.put('\n');
      buffer.go_left();
      save();
    }
    fflush(f);
    console_put_text("File saved.");
  }
}

void quit(int e) {
  exit(e);
}

void split(const literal &l) {
  auto &tab = get_current_tab();
  if(!l.data) {

  } else {
#if 0
    get_string_from_literal(filename, l);
    buffer_t buf;
    bool new_file = true;
    for(auto i = 0u; i < tab.buffers.size; i++) {
      if(tab.buffers[i].filename == e->path) {
        //buf.buffer = 
        new_file = false;
        break;
      }
    }
    /*
    FILE *f = get_file_or_create(filename, "r");
    defer { fclose(f); };
    */


    auto &b = tab.buffers.add(read_entire_file(f));
    auto &p = tab.buffers[0]; // @Hardcode:
    b.init(filename, Width - p.width/2, 0, p.width/2, p.height);
    p.init(p.filename, 0, 0, p.width/2, p.height);
    
    tab.active_buffer = &b;
#endif
  }
}
