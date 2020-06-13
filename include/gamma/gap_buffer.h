#ifndef GAMMA_GAP_BUFFER_H
#define GAMMA_GAP_BUFFER_H

#include "gamma/array.h"

template<class T>
struct gap_buffer {
  array<T> buf;
  unsigned pre_len;
  unsigned gap_len;


  gap_buffer(unsigned __pre=0)
    : pre_len{__pre}, gap_len{2} //  TODO: Check on the fastest default value for gap_len.
    {
    buf.resize(gap_len);
  }

  void insert(const T &val) {
    buf.push_back(val);
  }

  void insert(T &&val) {
    buf.push_back(std::move(val));
  }

  void move_right() {
    auto post_start = pre_len + gap_len;
    auto gap_start = pre_len;
    if(buf.size() == post_start) {
      return;
    }
    buf[gap_start] = buf[post_start-1];
    pre_len++;
  }

  void move_left() {
    auto post_start = pre_len + gap_len;
    auto gap_start = pre_len;
    if(gap_start == 0) {
      return;
    }
    buf[post_start-1] = buf[gap_start];
    pre_len--;
  }

  void add(T val) {
    auto gap_start = pre_len;

    if(gap_len == 0)  {
    /*
      Allocate array with new gap.
        1) allocate new array of (size*2).
        2) move gap left by post_len # of times.
      So from pre_len to pre_len+gap_len will be new gap.

      // [1, 2, 3, 4, 5]
      //       |
      //    [ ' ' ]
      //   gap(len=0).

      // [1, 2, 3, 4, 5, [' ', ' ', ' ', ' ', ' ']] <- resize.
      //                      new_gap(len=5)
      //              |            |
      //            pre_len     gap_len

      // [1, 2, [' ', ' ', ' ', ' ', ' '], 3, 4, 5]
      //         moving gap(3 times left).
    */
      auto size = buf.size();
      auto post_len = size - pre_len;
      pre_len = size;
      gap_len = size;

      buf.resize_with_no_init();
      for(unsigned i = 0; i < post_len; i++) {
        move_left();
      }
    }
    buf[gap_start] = val;
    pre_len++;
    gap_len--;
  }
};

using buffer_t = gap_buffer<gap_buffer<char>>;
#endif
