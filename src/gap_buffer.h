#ifndef GAMMA_GAP_BUFFER_H
#define GAMMA_GAP_BUFFER_H

#define INITIAL_GAP_LENGTH 1024


struct gap_buffer {
  string chars;
  size_t pre_len;
  size_t gap_len;


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

      if(chars.size) {
        size_t size, post_len;
        size     = chars.size;
        post_len = size - pre_len;
        pre_len  = size;
        gap_len  = size;

        chars.resize(size*2);
        for(size_t i = 0; i < post_len; i++) { move_left(); }

      } else {
        size_t size;
        size    = INITIAL_GAP_LENGTH;
        pre_len = 0;
        gap_len = size;
        chars.resize(size);
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
      backspace();
    }
    assert(pre_len == 0 && size() == 0);
  }

  char &operator[](size_t i) {
    assert(i < chars.size-gap_len);
    if(i < pre_len) {
      return chars[i];
    } else {
      return chars[i+gap_len];
    }
  }

  const char &operator[](size_t i) const { // Copy&Paste.
    assert(i < size());
    if(i < pre_len) {
      return chars[i];
    } else {
      return chars[i+gap_len];
    }
  }

  size_t size() const {
    return chars.size - gap_len;
  }
};

inline string string_from_gap_buffer(const gap_buffer *g) {
  const size_t size    = g->size();
  const size_t pre_len = g->pre_len;
  const size_t gap_len = g->gap_len;

  char *r = (char*)allocate(size+1);

  memcpy(r,           g->chars.data,                     pre_len);
  memcpy(r + pre_len, g->chars.data + pre_len + gap_len, size-pre_len);

  string s;
  s.data     = r;
  s.capacity = size+1;
  s.size     = size+1;
  return s;
}

inline void free_gap_buffer(gap_buffer *g) {
  free_string(&g->chars);
}

inline void copy_gap_buffer(gap_buffer *a, const gap_buffer *b) {
  *a = {};
  copy_string(&a->chars, &b->chars);
  a->pre_len = b->pre_len;
  a->gap_len = b->gap_len;
}

inline void move_gap_buffer(gap_buffer *a, gap_buffer *b) {
  copy_gap_buffer(a, b);
  free_gap_buffer(b);
  *b = {};
}

#undef INITIAL_GAP_LENGTH 

#endif
