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
    auto &p = add();
    p = val;
    return p;
  }

  T &add(T &&val) {
    auto &p = add();
    p = std::move(val);
    return p;
  }

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
    auto &p = insert(index);
    p = c;
    return p;
  }

  T &insert(T &&c, size_t index) {
    auto &p = insert(index);
    p = std::move(c);
    return p;
  }

  T &pop() {
    return data[--size];
  }

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

inline bool operator==(const array<char> &a, const array<char> &b) {
  if(a.size == b.size) {
    return strncmp(a.data, b.data, a.size);
  } else {
    return false;
  }
}

template<class T>
void copy_array(array<T> *a, const array<T> *b) {
  assert(b->size <= b->capacity);

  a->data = (T *)malloc(sizeof(T) * b->capacity);
  memcpy(a->data, b->data, sizeof(T) * b->size);

  a->capacity = b->capacity;
  a->size     = b->size;
}

template<class T>
void move_array(array<T> *a, const array<T> *b) {
  a->data     = std::move(b->data);
  a->capacity = std::move(b->capacity);
  a->size     = std::move(b->size);
}



inline void from_c_string(array<char> *s, const char *c_string, size_t size) {
  if(s->capacity < size) {
    s->resize_with_no_init(size);
  }
  memcpy(s->data, c_string, sizeof(char)*size);
}

inline void from_c_string(array<char> *s, const char *c_string) {
  from_c_string(s, c_string, strlen(c_string));
}

#define to_c_string(s, c_name) \
  char c_name[s.size+1]; \
  memcpy(c_name, s.data, sizeof(char)*s.size); \
  c_name[s.size] = '\0';

inline std::ostream &operator<<(std::ostream &os, const array<char> &s) {
  assert(s.size <= s.capacity);
  for(size_t i = 0; i < s.size; i++) {
    os << s[i];
  }
  return os;
}

#endif
