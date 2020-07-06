#ifndef GAMMA_GAP_BUFFER_H
#define GAMMA_GAP_BUFFER_H
#include "gamma/array.h"


// @Note:
// Gap_buffer working correct only if 
// buf.__size == buf.__capacity;
// Don't know whether it's good or not.

// @Note:
// In text editor it's everywhere used
// with an additional trailing space.
// Helps to handle cursor positions/moves.

template<class T>
struct gap_buffer {
  array<T> buf;
  unsigned pre_len;
  unsigned gap_len;

public:
  gap_buffer(unsigned __pre=0)
    : pre_len{__pre}, gap_len{12} //  TODO: Check on the fastest default value for gap_len.
    {
    buf.resize_with_no_init(pre_len+gap_len);
  }

  gap_buffer(T val): gap_buffer{} {
    insert(val);
  }

  void insert(const T &val) {
    buf.push_back(val);
  }

  void insert(T &&val) {
    buf.push_back(std::move(val));
  }

  void insert_many(const array<T> &other) {
    for(unsigned i = 0; i < other.size(); i++) {
      buf.push_back(other[i]);
    }
    buf.resize_with_no_init(buf.size());
  }

  void move_right() {
    auto post_start = pre_len + gap_len;
    auto gap_start = pre_len;
    if(buf.size() == post_start) {
      return;
    }
    buf[gap_start] = buf[post_start];
    pre_len++;
  }

  void move_left() {
    auto post_start = pre_len + gap_len;
    auto gap_start = pre_len;
    if(gap_start == 0) {
      return;
    }
    buf[post_start-1] = buf[gap_start-1];
    pre_len--;
  }

  void add(const T &val) {
    auto &gap_start = pre_len;

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

  void backspace() {
    // [1, 2, [' ', ' '], 3, 4, 5]
    // [1, [' ', ' ', ' '], 3, 4, 5] // backspace.
    if(pre_len == 0) return;
    pre_len--;
    gap_len++;
  }

  void del() {
    // [1, 2, [' ', ' '], 3, 4, 5]
    // [1, 2, [' ', ' ', ' '], 4, 5] // delete.
    auto post_len = buf.size() - pre_len - gap_len;
    if(post_len == 1) return; // also 1 extra space.
    gap_len++;
  }

  T &operator[](unsigned i) {
    assert(i >= 0 && i < buf.size()-gap_len);
    if(i < pre_len) {
      return buf[i];
    } else {
      return buf[i+gap_len];
    }
  }

  const T &operator[](unsigned i) const {
    // Copy&Paste.
    assert(i >= 0 && i < buf.size()-gap_len);
    if(i < pre_len) {
      return buf[i];
    } else {
      return buf[i+gap_len];
    }
  }

  unsigned size() const {
    return buf.size() - gap_len;
  }


  void move_right_by(unsigned i) {
    while(i) {
      move_right();
      i--;
    }
  }

  void move_left_by(unsigned i) {
    while(i) {
      move_left();
      i--;
    }
  }

  void resize(unsigned size_to_resize=0) {
    if(!size_to_resize) {
      size_to_resize = buf.capacity() * 2;
    }
    buf.resize(size_to_resize);
  }
};

template<class T>
bool operator==(const gap_buffer<T> &t, const gap_buffer<T> &b) {
  auto size = t.size();
  if(t.size() != b.size()) {
    return false;
  }
  for(unsigned i = 0; i < size; i++) {
    if(t[i] != b[i]) {
      return false;
    }
  }
  return true;
}


#endif
