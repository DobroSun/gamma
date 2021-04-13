#ifndef GAMMA_ARRAY_H
#define GAMMA_ARRAY_H

// 
template<class T>
struct array {
  T     *data     = NULL;
  size_t size     = 0;
  size_t capacity = 0;


  T* add() {
    if(size == capacity) { reserve(); }
    new (&data[size]) T();
    return &data[size++];
  }

  T* add(T v) {
    return &(*add() = v);
  }

  T* insert(size_t index) {
    if(index >= size)    { return add(); }
    if(size == capacity) { reserve(); }

    const size_t size_to_copy = size - index;
    size++;

    T tmp[size_to_copy];
    memcpy(tmp, data+index, sizeof(tmp));
    memcpy(data+index+1, tmp, sizeof(tmp));

    new (&data[index]) T();
    return &data[index];
  }

  template<class U, class B> // U for different types, B for constness.
  void find(U v, B **iter, size_t *index) {
    for(size_t i = 0; i < size; i++) {
      if(this->operator[](i) == v) {
        *iter  = &this->operator[](i);
        *index = i;
        return;
      }
    }
    *iter = NULL;
  }

  template<class U, class B>
  void find(U v, B **iter) { // @Copy&Paste:
    for(size_t i = 0; i < size; i++) {
      if(this->operator[](i) == v) {
        *iter = &this->operator[](i);
        return;
      }
    }
    *iter = NULL;
  }

  template<class U, class B>
  void find_pointer(U *v, B **iter, size_t *index) { // @Copy&Paste: We can reduce this boilerplate by doing some compiler time indirection (probably).
    for(size_t i = 0; i < size; i++) {
      if(&this->operator[](i) == v) {
        *iter  = &this->operator[](i);
        *index = i;
        return;
      }
    }
    *iter = NULL;
  }

  T* remove(size_t index) {
    if(index >= size) { return &data[size--]; }

    T* r = &data[index];

    size--;
    const size_t size_to_copy = size - index;

    T tmp[size_to_copy];
    memcpy(tmp, data+index+1, sizeof(tmp));
    memcpy(data+index, tmp, sizeof(tmp));
    return r;
  }


  T* pop() { return &data[--size]; }

  T& first()             { return data[0]; }
  T& last()              { return data[size-1]; }
  const T& first() const { return data[0]; }
  const T& last()  const { return data[size-1]; }

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

        memcpy(new_data, data, sizeof(T)*size);
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

  void resize(size_t new_size=0) { // @Incomplete: @Wrong: Needs to call all the default constructors.
    reserve(new_size);
    size = capacity;
  }

  void clear() {
    size = 0;
  }

  bool empty() const {
    return size == 0;
  }

  T& operator[](size_t index) {
    assert(index < size);
    return data[index];
  }

  const T& operator[](size_t index) const {
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
  iterator begin() const { return iterator(0, data); }
  iterator end()   const { return iterator(size);    }
};

template<class T>
void copy_array(array<T> *a, const array<T> *b) {
  if(a->capacity <= b->size) {
    free(a->data);
    a->data     = (T*)malloc(sizeof(T)*b->capacity);
    a->capacity = b->capacity;
  }
  memcpy(a->data, b->data, sizeof(T)*b->size);
  a->size = b->size;
}

template<class T>
void free_array(array<T> *a) { free(a->data); }
// 




// dynamic string.
struct string : array<char> {
  string() = default;
  string(const char *x, size_t N) {
    data     = (char*)calloc(N+1, 1);
    memcpy(data, x, N);
    capacity = N+1;
    size     = N;
  }

  string(const char *x) {
    const size_t N = strlen(x);
    data     = (char*)calloc(N+1, 1);
    memcpy(data, x, N);
    capacity = N+1;
    size     = N;
  }

  // @Copy&Paste: of array::reserve.
  void reserve(size_t new_cap=0) {
    if(!data) {
      assert(!capacity && !size && !data);

      new_cap  = (new_cap)? new_cap: 8;
      ++new_cap;                              // null terminator.
      data     = (char*)calloc(new_cap, 1);
      capacity = new_cap;

    } else {
      assert(data && capacity);
      new_cap = (new_cap)? new_cap: 2*capacity;
      ++new_cap;                              // null terminator.

      if(new_cap > capacity) {
        auto new_data = (char*)calloc(new_cap, 1);

        if(size) { memcpy(new_data, data, size); }
        capacity = new_cap;

        free(data);
        data = new_data;
      } else {
        // There is no point in shrinking down the allocation. (I think).
      }
    }
  }

  void resize(size_t new_cap) {
    reserve(new_cap+1);
    assert(data[capacity-1] == '\0');
    size = capacity-2;
  }
};

inline std::ostream &operator<<(std::ostream &os, string s) {
  for(const auto &c : s) { os << c; } return os;
  
}

inline bool operator==(string b, const char *a) { return !strncmp(a, b.data, b.size); }
inline bool operator==(const char *a, string b) { return !strncmp(a, b.data, b.size); }
inline bool operator==(string a, string b) {
  if(a.size == b.size) {
    return !strncmp(a.data, b.data, a.size);
  } else {
    return false;
  }
}

inline void copy_string(string *a, const string *b) { copy_array(a, b); }
inline void free_string(string *s)                  { free_array(s); }
// 


#endif
