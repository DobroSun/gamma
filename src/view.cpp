#include "gamma/pch.h"
#include "gamma/gap_buffer.h"
#include "gamma/view.h"

static gap_buffer<char> empty = ' ';

buffer_view::buffer_view(buffer_t &view, unsigned __start)
                         : v{view}, start{__start}
                         {}



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
  auto index = start+i;
  if(index < v.size()) {
    return v[index];
  } else {
    return empty;
  }
}

// @Copy&Paste.
const gap_buffer<char> &buffer_view::operator[](unsigned i) const {
  auto index = start+i;
  if(index < v.size()) {
    return v[index];
  } else {
    return empty;
  }
}

void buffer_view::increase_start_by(unsigned i) {
  start += i;
  v.move_right_by(i);
}

void buffer_view::decrease_start_by(unsigned i) {
  start -= i;
  v.move_left_by(i);
}

unsigned buffer_view::size() const {
  return v.size();
}

unsigned buffer_view::pre_len() const {
  return v.pre_len;
}
