#ifndef GAMMA_VIEW_H
#define GAMMA_VIEW_H
#include "gamma/fwd_decl.h"
#include "gamma/gap_buffer.h"
#include "gamma/globals.h"


struct string_view {
  const gap_buffer<char> &v;
  unsigned offset{0};

  string_view(const gap_buffer<char> &, unsigned);

  char operator[](unsigned);
  const char operator[](unsigned) const;

  unsigned size() const;
};

struct buffer_view {
  buffer_t &v;
  gap_buffer<char> console;
  unsigned start{0}, start_j{0}, saved_j{0};
  Cursor cursor{0,0}; // @Note: This might be just a vector2 of ints.


  buffer_view(buffer_t &);

  void move_right();
  void move_left();
  void add(const gap_buffer<char> &);
  void backspace();
  void del();

  gap_buffer<char> &operator[](unsigned);
  const gap_buffer<char> &operator[](unsigned) const;
  const string_view get_view(unsigned) const;

  void increase_start_by(int);
  void decrease_start_by(int);

  unsigned size() const;
  unsigned pre_len() const;
};

#endif
