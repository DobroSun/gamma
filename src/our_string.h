#ifndef GAMMA_STRING_IMPL_H
#define GAMMA_STRING_IMPL_H



struct string {
  char *data     = NULL;
  s32   capacity = 0;
  s32   size     = 0;


  string() = default;

  string(const char *x) {
    const size_t N = strlen(x);
    data     = (char*)calloc(N+1, 1);
    memcpy(data, x, N);
    capacity = N+1;
    size     = N;
  }

  string(const char *x, size_t N) {
    data     = (char*)calloc(N+1, 1);
    memcpy(data, x, N);
    capacity = N+1;
    size     = N;
  }

  string(const string &o)            = default;
  string &operator=(const string &o) = default;
  string(string &&o)                 = default;
  string &operator=(string &&o)      = default;

  ~string() { new (this) string(); }

  void find(char c, const char **iter, size_t *index) const {
    for(size_t i = 0; i < size; i++) {
      if(this->operator[](i) == c) {
        *iter  = &this->operator[](i);
        *index = i;
        return;
      }
    }
    *iter = NULL;
  }

  void add(char c) {
    if(size == capacity) { reserve(); }
    data[size++] = c;
  }

  char &pop() { 
    assert(size > 0);
    return data[--size];
  }

  char &operator[](s32 index) {
    assert(index >= 0 && index < size);
    return data[index];
  }

  const char &operator[](s32 index) const {
    assert(index >= 0 && index < size);
    return data[index];
  }

  void reserve(s32 new_cap=0) {
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

  void resize(s32 new_cap=0) {
    reserve(new_cap+1);
    assert(data[capacity-1] == '\0');
    size = capacity-2;
  }

  void  clear()       { size = 0; }
  bool  empty() const { return size == 0; }

        char &first()       { return data[0]; }
  const char &first() const { return data[0]; }
        char &last()        { return data[size-1]; }
  const char &last() const  { return data[size-1]; }


  struct iterator {
    s32 index; char *p;
    
    explicit iterator(s32 i)          { index = i; }
    explicit iterator(s32 i, char* d) { index = i; p = d; }
    iterator& operator++()            { ++index; return *this; }
    iterator& operator++(int)         { ++index; return *this; }
    bool operator==(iterator o) const { return index == o.index; }
    bool operator!=(iterator o) const { return index != o.index; }
    char& operator*()           const { return p[index]; }
  };
  iterator begin() const { return iterator(0, data); }
  iterator end()   const { return iterator(size);    }
};

inline std::ostream &operator<<(std::ostream &os, string s) {
  for(const auto &c : s) { os << c; }
  return os;
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

inline void copy_string(string &a, string b) {
  if(a.capacity <= b.size) {
    free(a.data);
    a.data = (char*)malloc(b.size);
    a.capacity = b.size;
  }
  memcpy(a.data, b.data, b.size);
  a.size = b.size;
}

inline void free_string(string &s) { free(s.data); }

#endif
