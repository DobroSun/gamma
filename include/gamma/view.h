#ifndef GAMMA_VIEW_H
#define GAMMA_VIEW_H
#include "gamma/fwd_decl.h"
#include "gamma/cursor.h"


struct buffer_view {
  buffer_t &v;
  unsigned start;
  Cursor cursor;


  buffer_view(buffer_t &);

  void move_right();
  void move_left();
  void add(const gap_buffer<char> &);
  void backspace();
  void del();

  gap_buffer<char> &operator[](unsigned);
  const gap_buffer<char> &operator[](unsigned) const;

  void increase_start_by(int);
  void decrease_start_by(int);

  void move_right_by(int);

  unsigned size() const;
  unsigned pre_len() const;
};

#endif
