#ifndef GAMMA_GAP_BUFFER_H
#define GAMMA_GAP_BUFFER_H

struct gap_buffer {
  string_t chars;
  size_t pre_len = 0;
  size_t gap_len = 12; // @SpeedUp: Check on the best default value for gap_len.


  gap_buffer() = default;

  gap_buffer(const gap_buffer &other) = delete;
  gap_buffer &operator=(const gap_buffer &other) = delete;
  gap_buffer(gap_buffer &&other) = delete;
  gap_buffer &operator=(gap_buffer &&other) = delete;


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

      assert(chars.capacity == size);
      chars.resize_with_no_init(size*2);
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
    assert(empty());
  }

  char &operator[](size_t i) {
    assert(is_initialized() && i < chars.size-gap_len);
    if(i < pre_len) {
      return chars[i];
    } else {
      return chars[i+gap_len];
    }
  }

  const char &operator[](size_t i) const {
    // Copy&Paste.
    assert(is_initialized() && i < chars.size-gap_len);
    if(i < pre_len) {
      return chars[i];
    } else {
      return chars[i+gap_len];
    }
  }

  size_t size() const {
    assert(is_initialized());
    return chars.size - gap_len;
  }

  bool empty() const {
    assert(is_initialized());
    return size() == 0;
  }

  bool is_initialized() const {
    return chars.data != NULL;
  }

  void to_string(char *dst, size_t s) { // @Now.
    size_t i = 0;
    for( ; i < size(); i++) {
      if(i == s) break;
      dst[i] = (*this)[i];
    }
    dst[i] = '\0';
  }
};

inline void copy_gap_buffer(gap_buffer *a, const gap_buffer *b) {
  copy_array(&a->chars, &b->chars);
  a->pre_len = b->pre_len;
  a->gap_len = b->gap_len;
}

inline void move_gap_buffer(gap_buffer *a, gap_buffer *b) {
  move_array(&a->chars, &b->chars);
  a->pre_len = b->pre_len;
  a->gap_len = b->gap_len;

  assert(b->chars.data == NULL);
  b->pre_len = 0;
  b->gap_len = 0;
}

#endif
