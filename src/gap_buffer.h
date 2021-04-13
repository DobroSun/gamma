#ifndef GAMMA_GAP_BUFFER_H
#define GAMMA_GAP_BUFFER_H

struct gap_buffer {
  string chars;
  size_t pre_len = 0;
  size_t gap_len = 12; // @Speed: Check on the best default value for gap_len.


  void move_right() {
    auto post_start = pre_len + gap_len;
    if(chars.size == post_start) {
      return;
    }
    chars[pre_len++] = chars[post_start];
  }

  void move_left() {
    auto post_start = pre_len + gap_len;
    if(pre_len == 0) {
      return;
    }
    chars[post_start-1] = chars[--pre_len];
  }

  void move_until(size_t new_pre_len) {
    if(new_pre_len == pre_len) {
      // Nothing.

    } else if(new_pre_len > pre_len) {
      int diff = new_pre_len - pre_len;
      while(diff) {
        move_right();
        diff--;
      }

    } else {
      assert(new_pre_len < pre_len);
      int diff  = pre_len - new_pre_len;
      while(diff) {
        move_left();
        diff--;
      }
    }
    assert(new_pre_len == pre_len);
  }

  void add(char val) {
    if(gap_len == 0)  {
      /*
      Allocate chars with new gap.
        1) allocate new chars of (size*2).
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

      size_t size     = chars.size;
      size_t post_len = size - pre_len;
      pre_len = size;
      gap_len = size;

      chars.resize(size*2);
      for(size_t i = 0; i < post_len; i++) {
        move_left();
      }
    }

    chars[pre_len++] = val;
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
    auto post_len = chars.size - pre_len - gap_len;
    if(post_len == 0) return;
    gap_len++;
  }

  void clear() {
    auto post_len = chars.size - pre_len - gap_len;
    del();
    while(post_len > 1) {
      del();
      post_len--;
    }
    while(pre_len) {
      backspace(); // decreases pre_len by itself.
    }
    assert(!pre_len);
    assert(size() == 0);
  }

  char &operator[](size_t i) {
    assert(i < chars.size-gap_len);
    if(i < pre_len) {
      return chars[i];
    } else {
      return chars[i+gap_len];
    }
  }

  const char &operator[](size_t i) const {
    // Copy&Paste.
    assert(i < chars.size-gap_len);
    if(i < pre_len) {
      return chars[i];
    } else {
      return chars[i+gap_len];
    }
  }

  size_t size() const { return chars.size - gap_len; }
};

inline bool operator==(const gap_buffer &a, const gap_buffer &b) {
  if(a.size() == b.size()) {
    for(size_t i = 0; i < a.size(); i++) {
      if(a[i] != b[i]) {
        return false;
      }
    }
    return true;
  } else {
    return false;
  }
}

inline bool operator!=(const gap_buffer &a, const gap_buffer &b) { return !(a == b); }

inline void copy_gap_buffer(gap_buffer *a, const gap_buffer *b) {
  new (a) gap_buffer(); // this handles the case, when a & b point to the same gap_buffer.
  copy_string(&a->chars, &b->chars);
  a->pre_len = b->pre_len;
  a->gap_len = b->gap_len;
}

#endif
