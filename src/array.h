#ifndef GAMMA_ARRAY_H
#define GAMMA_ARRAY_H

template<class T>
struct array {
  T     *data     = NULL;
  size_t capacity = 0;
  size_t size     = 0;

  array() = default;
  array(const array &)       = default;
  array& operator=(array &)  = default;
  array(array &&)            = default;
  array& operator=(array &&) = default;

  ~array() { new (this) array<T>(); }


  T &add() {
    if(size == capacity) { reserve(); }
    new (&data[size]) T();
    return data[size++];
  }

  T &add(const T &val) { return add() = val; }
  T &add(T &&val)      { return add() = std::move(val); }

  template<class U>
  void find(const U &val, T **iter, size_t *index) {
    for(size_t i = 0; i < size; i++) {
      if(this->operator[](i) == val) {
        *iter  = &this->operator[](i);
        *index = i;
        return;
      }
    }
    *iter = NULL;
  }

  template<class U>
  void find(const U &val, T **iter) {
    for(size_t i = 0; i < size; i++) {
      if(this->operator[](i) == val) {
        *iter = &this->operator[](i);
        return;
      }
    }
    *iter = NULL;
  }

  T &insert(size_t index) {
    if(index >= size) { return add(); }

    if(size == capacity) { reserve(); }

    const size_t size_to_copy = size - index;
    size++;

    T tmp[size_to_copy];

    memcpy(tmp, data+index, sizeof(tmp));
    memcpy(data+index+1, tmp, sizeof(tmp));

    return data[index];
  }

  T &insert(const T &c, size_t index) {
    return insert(index) = c;
  }

  T &insert(T &&c, size_t index) {
    return insert(index) = std::move(c);
  }


  T &pop()   { return data[--size]; }
  T &first() { return data[0]; }
  T &last()  { return data[size-1]; }

  const T &first() const { return data[0]; }
  const T &last()  const { return data[size-1]; }

  void reserve(size_t new_cap=0) {
    if(!data) {
      assert(!capacity && !size && !data);

      new_cap = (new_cap)? new_cap: 8;
      data = (T*)malloc(sizeof(T)*new_cap);
      capacity = new_cap;

    } else {
      assert(data && capacity);
      new_cap = (new_cap)? new_cap: 2*capacity;

      if(new_cap > capacity) {
        auto new_data = (T*)malloc(sizeof(T)*new_cap);
        assert(new_data);

        if(size) {
          memcpy(new_data, data, sizeof(T)*size);
        }
        capacity = new_cap;

        free(data);
        data = new_data;

      } else {
        assert(new_cap <= capacity);
        if(size > new_cap) {
          size     = new_cap;
          capacity = new_cap;

          auto new_data = (T*)malloc(sizeof(T)*new_cap);
          assert(new_data);

          memcpy(new_data, data, sizeof(T)*size);

          free(data);
          data = new_data;

        } else {
          // Do nothing.
        }
      }
    }
  }

  void resize(size_t new_size=0) {
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

  struct iterator {
    size_t index; T *p;
    
    explicit iterator(size_t i)       { index = i; }
    explicit iterator(size_t i, T* d) { index = i; p = d; }
    iterator& operator++()            { ++index; return *this; }
    iterator& operator++(int)         { ++index; return *this; }
    bool operator==(iterator o) const { return index == o.index; }
    bool operator!=(iterator o) const { return index != o.index; }
    T& operator*()              const { return p[index]; }
  };
  iterator begin() { return iterator(0, data); }
  iterator end()   { return iterator(size);    }
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

template<class T>
void copy_array(array<T> &a, const array<T> &b) {
  if(a.capacity <= b.size) {
    free(a.data);
    a.data = (T*)malloc(sizeof(T)*b.size);
    a.capacity = b.size;
  } else {
  }

  memcpy(a.data, b.data, sizeof(T)*b.size);
  a.size = b.size;
}

template<class T>
void free_array(array<T> &a) { free(a.data); }

#endif
