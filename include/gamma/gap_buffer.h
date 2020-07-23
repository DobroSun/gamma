#ifndef GAMMA_GAP_BUFFER_H
#define GAMMA_GAP_BUFFER_H

// @Note1:
// Gap_buffer working correct only if 
// buf.__size == buf.__capacity;

// @Note2:
// In text editor it's everywhere used
// with an additional trailing space.
// Helps to handle cursor positions/moves.

template<class T>
struct gap_buffer {
  array<T> buf;
  unsigned pre_len{0};
  unsigned gap_len{12}; //  TODO: Check on the fastest default value for gap_len.

public:
  gap_buffer() {
    buf.resize_with_no_init(pre_len+gap_len);
  }

  gap_buffer(const T &val): gap_buffer{} {
    add(val);
    move_left();
  }

  void insert_many(const array<T> &other) {
    for(unsigned i = 0; i < other.size(); i++) {
      buf.push_back(other[i]);
    }
    buf.resize_with_no_init(buf.size());
  }

  void move_right() {
    auto post_start = pre_len + gap_len;
    if(buf.size() == post_start) {
      return;
    }
    buf[pre_len++] = buf[post_start];
  }

  void move_left() {
    auto post_start = pre_len + gap_len;
    if(pre_len == 0) {
      return;
    }
    buf[post_start-1] = buf[--pre_len];
  }

  void add(const T &val) {
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
      unsigned size = buf.size();
      unsigned post_len = size - pre_len;
      pre_len = size;
      gap_len = size;

      assert(buf.capacity() == size);
      buf.resize_with_no_init(size*2);
      for(unsigned i = 0; i < post_len; i++) {
        move_left();
      }
    }

    buf[pre_len++] = val;
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

  void clear() {
    auto post_len = buf.size() - pre_len - gap_len;
    while(post_len > 1) {
      del();
      post_len--;
    }
    while(pre_len) {
      backspace(); // decreases pre_len by itself.
    }
    assert(!pre_len);
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

inline char *to_c_string(gap_buffer<char> &t) {
	char *ret = (char *)malloc(sizeof(char) * t.size() + 1);
	unsigned i = 0;
  for(; i < t.size(); i++) {
    ret[i] = t[i];
  }
	ret[i] = '\0';
  return ret;
}
#endif
