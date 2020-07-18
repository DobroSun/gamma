#include "gamma/pch.h"
#include "gamma/buffer.h"
#include "gamma/gap_buffer.h"
#include "gamma/view.h"

static buffer_view buffer;

buffer_view &get_buffer() {
  return buffer;
}

bool load_buffer_from_file(const string &filename) {
  auto &b = buffer.v;

  auto source_file = fopen(filename.c_str(), "r");
  if(!source_file) {
    return false;
  }

  int buffer_length = 255;
  char tmp[buffer_length];
  while(fgets(tmp, buffer_length, source_file)) {
    gap_buffer<char> g;

    for(unsigned i = 0; i < strlen(tmp)-1; i++) {
      g.insert(tmp[i]);
    }

    g.insert(' ');
    b.insert(g);
  }

  fclose(source_file);
  return true;
}
