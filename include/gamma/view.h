#ifndef GAMMA_VIEW_H
#define GAMMA_VIEW_H
#include "gamma/fwd_decl.h"


struct buffer_view {
  buffer_t &v;
  unsigned start;


  buffer_view(buffer_t &view, unsigned __start=0);

  unsigned size() const;
  void move_right();
  void move_left();
  void add(const gap_buffer<char> &val);

  gap_buffer<char> &operator[](unsigned i);
  const gap_buffer<char> &operator[](unsigned i) const;

};

#endif
