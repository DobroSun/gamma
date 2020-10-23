#ifndef GAMMA_GAP_BUFFER_H
#define GAMMA_GAP_BUFFER_H

struct gap_buffer {
  array<char> chars;
  unsigned pre_len = 0;
  unsigned gap_len = 12; // @SpeedUp: Check on the best default value for gap_len.


  gap_buffer() {
    chars.resize_with_no_init(gap_len);
  }

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
      unsigned size = chars.size;
      unsigned post_len = size - pre_len;
      pre_len = size;
      gap_len = size;

      assert(chars.capacity == size);
      chars.resize_with_no_init(size*2);
      for(unsigned i = 0; i < post_len; i++) {
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

  char &operator[](unsigned i) {
    assert(i < chars.size-gap_len);
    if(i < pre_len) {
      return chars[i];
    } else {
      return chars[i+gap_len];
    }
  }

  const char &operator[](unsigned i) const {
    // Copy&Paste.
    assert(i < chars.size-gap_len);
    if(i < pre_len) {
      return chars[i];
    } else {
      return chars[i+gap_len];
    }
  }

  unsigned size() const {
    return chars.size - gap_len;
  }

  void to_string(char *dst, unsigned s) {
    unsigned i = 0;
    for( ; i < size(); i++) {
      if(i == s) break;
      dst[i] = (*this)[i];
    }
    dst[i] = '\0';
  }
};

#endif
