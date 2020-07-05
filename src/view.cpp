#include "gamma/pch.h"
#include "gamma/gap_buffer.h"
#include "gamma/view.h"

static char space = ' ';
static gap_buffer<char> empty = space;

string_view::string_view(const gap_buffer<char> &view, unsigned o): v{view}, offset{o} {}


char string_view::operator[](unsigned i) {
  if(i < v.size()) {
    return v[i];
  } else {
    return space;
  }
}

const char string_view::operator[](unsigned i) const {
  if(i < v.size()) {
    return v[i];
  } else {
    return space;
  }
}

unsigned string_view::size() const {
  return v.size();
}


buffer_view::buffer_view(buffer_t &view): v{view} {}

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

gap_buffer<char> &buffer_view::operator[](unsigned i) {
  if(i < v.size()) {
    return v[i];
  } else {
    return empty;
  }
}

// @Copy&Paste.
const gap_buffer<char> &buffer_view::operator[](unsigned i) const {
  if(i < v.size()) {
    return v[i];
  } else {
    return empty;
  }
}

const string_view buffer_view::get_view(unsigned i) const {
  return string_view{this->operator[](i), start_j};
}

void buffer_view::increase_start_by(int i) {
  assert(i >= 0);
  start += i;
  v.move_right_by(i);
}

void buffer_view::decrease_start_by(int i) {
  assert(i >= 0);
  start -= i;
  v.move_left_by(i);
}

unsigned buffer_view::size() const {
  return v.size();
}

unsigned buffer_view::pre_len() const {
  return v.pre_len;
}

