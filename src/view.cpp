#include "gamma/pch.h"
#include "gamma/gap_buffer.h"
#include "gamma/view.h"


buffer_view::buffer_view(buffer_t &view, unsigned __start)
                         : v{view}, start{__start}
                         {}


unsigned buffer_view::size() const {
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

gap_buffer<char> &buffer_view::operator[](unsigned i) {
  assert(start+i < v.size());
  return v[start+i];
}

// @Copy&Paste.
const gap_buffer<char> &buffer_view::operator[](unsigned i) const {
  assert(start+i < v.size());
  return v[start+i];
}
