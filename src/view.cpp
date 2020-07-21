#include "gamma/pch.h"
#include "gamma/view.h"

static char space = ' ';
static gap_buffer<char> empty = space;

string_view::string_view(const gap_buffer<char> &view, unsigned o): v{view}, offset{o} {}


char string_view::operator[](unsigned i) const {
  if(i < v.size()) {
    return v[i];
  } else {
    return space;
  }
}

unsigned string_view::size() const {
  return v.size();
}


void buffer_view::move_right() {
  v.move_right();
}

void buffer_view::move_left() {
  v.move_left();
}

void buffer_view::add(const gap_buffer<char> &val) {
  v.add(val);
}

void buffer_view::backspace() {
  v.backspace();
}

void buffer_view::del() {
  v.del();
}

void buffer_view::clear() {
  for(unsigned i = 0; i < v.size(); i++) {
    v[i].clear();
  }
  v.clear();
  console.clear();
  start = 0; start_j = 0; saved_j = 0;
  cursor = {0,0};
}

gap_buffer<char> &buffer_view::operator[](unsigned i) {
  if(i < v.size()) {
    return v[i];
  } else {
    return empty;
  }
}

const gap_buffer<char> &buffer_view::operator[](unsigned i) const {
  // @Copy&Paste.
  if(i < v.size()) {
    return v[i];
  } else {
    return empty;
  }
}

const string_view buffer_view::get_view(unsigned i) const {
  return string_view{this->operator[](i), start_j};
}

unsigned buffer_view::size() const {
  return v.size();
}
