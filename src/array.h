#ifndef GAMMA_ARRAY_H
#define GAMMA_ARRAY_H


template<class T>
struct array {
  T *data = NULL;
  size_t capacity = 0;
  size_t size = 0;

  static constexpr size_t eight = 8;


  array() = default;

  array(const array &other) = delete;
  array &operator=(const array &other) = delete;
  array(array &&other) = delete;
  array &operator=(array &&other) = delete;

  ~array() {
    free(data);
    data     = NULL;
    capacity = 0;
    size     = 0;
  }

  T &add() {
    if(size == capacity) { reserve(); }
    return data[size++];
  }

  T &add(const T &val) {
    return add() = val;
  }

  T &add(T &&val) {
    return add() = std::move(val);
  }

#if 0
  T &insert(size_t index) {
    assert(index < size);

    if(size == capacity) { reserve(); }

    const size_t copied_size = size - index;
    T tmp[copied_size];
    memcpy(tmp, data + copied_size, sizeof(tmp));
    memcpy(data + copied_size + 1, tmp, sizeof(tmp));
    return data[index];
  }

  T &insert(const T &c, size_t index) {
    return insert(index) = c;
  }

  T &insert(T &&c, size_t index) {
    return insert(index) = std::move(c);
  }
#endif
  T &pop()   { return data[--size]; }
  T &first() { return data[0]; }
  T &last()  { return data[size-1]; }

  void reserve(size_t new_cap=0) {
    if(!data) {
      assert(!capacity && !size && !data);

      new_cap = (new_cap)? new_cap: eight;
      data = (T*)malloc(sizeof(T) * new_cap);
      capacity = new_cap;

    } else {
      assert(data && capacity);
      new_cap = (new_cap)? new_cap: 2*capacity;

      if(new_cap > capacity) {
        auto new_data = (T*)malloc(sizeof(T) * new_cap);
        assert(new_data);
        if(size) {
          memcpy(new_data, data, sizeof(T) * size);
        }
        capacity = new_cap;

        free(data);
        data = new_data;

      } else {
        assert(new_cap <= capacity);
        if(size > new_cap) {
          size     = new_cap;
          capacity = new_cap;

          auto new_data = (T*)malloc(sizeof(T) * new_cap);
          assert(new_data);

          memcpy(new_data, data, sizeof(T) * size);

          free(data);
          data = new_data;

        } else {
          // Do nothing.
        }
      }
    }
  }

  void resize_with_no_init(size_t new_size=0) {
    reserve(new_size);
    size = capacity;
  }

  void clear() {
    size = 0;
  }

  bool empty() const {
    return size == 0;
  }

  T &operator[](size_t index) {
    assert(index < size);
    return data[index];
  }

  const T &operator[](size_t index) const {
    assert(index < size);
    return data[index];
  }
};

template<class T>
bool operator==(const array<T> &a, const array<T> &b) {
  if(a.size == b.size) {
    for(size_t i = 0; i < a.size; i++) {
      if(a[i] != b[i]) { return false; }
    }
    return true;
  } else {
    return false;
  }
}

inline bool operator==(const string_t &a, const string_t &b) {
  if(a.size == b.size) {
    return !strncmp(a.data, b.data, a.size);
  } else {
    return false;
  }
}

template<class T>
void copy_array(array<T> *a, const array<T> *b) {
  if(a->capacity <= b->size) {
    free(a->data);
    a->data = (T *)malloc(sizeof(T) * b->size);
    a->capacity = b->size;
  } else {
  }

  memcpy(a->data, b->data, sizeof(T) * b->size);
  a->size     = b->size;
}

template<class T>
void move_array(array<T> *a, array<T> *b) {
  a->data     = b->data;
  a->capacity = b->capacity;
  a->size     = b->size;

  b->data = NULL;
  b->capacity = 0;
  b->size     = 0;
}



inline void from_c_string(string_t *s, const char *c_string, size_t size) {
  if(s->capacity < size) {
    s->resize_with_no_init(size);
  }
  memcpy(s->data, c_string, sizeof(char)*size);
}

inline void from_c_string(string_t *s, const char *c_string) {
  from_c_string(s, c_string, strlen(c_string));
}

#define to_c_string(s, c_name) \
  char c_name[(s)->size+1]; \
  memcpy(c_name, (s)->data, sizeof(char) * (s)->size); \
  c_name[(s)->size] = '\0';


#define move_string(s1, s2) move_array(s1, s2)
#define copy_string(s1, s2) copy_array(s1, s2)

inline std::ostream &operator<<(std::ostream &os, const string_t &s) {
  assert(s.size <= s.capacity);
  for(size_t i = 0; i < s.size; i++) {
    os << s[i];
  }
  return os;
}

#endif
