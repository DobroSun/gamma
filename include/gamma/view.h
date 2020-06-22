#ifndef GAMMA_VIEW_H
#define GAMMA_VIEW_H
#include "gamma/fwd_decl.h"


struct buffer_view {
  buffer_t &v;
  unsigned start;
  

  buffer_view(buffer_t &view, unsigned __start=0);

  void move_right();
  void move_left();
  void add(const gap_buffer<char> &val);
  void backspace();
  void del();

  gap_buffer<char> &operator[](unsigned i);
  const gap_buffer<char> &operator[](unsigned i) const;

  void increase_start_by(unsigned);
  void decrease_start_by(unsigned);



  unsigned size() const;
  unsigned pre_len() const;
};

#endif
