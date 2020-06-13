#ifndef GAMMA_VIEW_H
#define GAMMA_VIEW_H

// TODO: Check performance on copy/move/ref.
template<class T>
struct vector_view {
  std::vector<T> &v;
  unsigned start;

public:
  vector_view(std::vector<T> &view, unsigned __start=0): v{view}, start{__start} {}

  vector_view(const vector_view<T> &) = delete;
  vector_view(vector_view<T> &&) = delete;
  vector_view &operator=(const vector_view<T> &) = delete;
  vector_view &operator=(vector_view<T> &&) = delete;

  const T &operator[](unsigned i) const {
    assert(start+i < v.size());
    return v[start+i];
  }

  T &operator[](unsigned i) {
    assert(start+i < v.size());
    return v[start+i];
  }

  int size() const {
    return v.size();
  }
};

template<>
struct vector_view<std::string> {
  std::string &v;
  unsigned start;

public:
  vector_view(std::string &view, unsigned __start=0): v{view}, start{__start} {}

  vector_view(const std::string &) = delete;
  vector_view(std::string &&) = delete;
  vector_view &operator=(const std::string &) = delete;
  vector_view &operator=(std::string &&) = delete;

  const char &operator[](unsigned i) const {
    assert(start+i < v.size());
    return v[start+i];
  }

  char &operator[](unsigned i) {
    assert(start+i < v.size());
    return v[start+i];
  }

  char at_or(unsigned i, char val) const {
    if(start+i < v.size()) {
      return this->operator[](i);
    }
    return val;
  }

  int size() const {
    return v.size();
  }

  std::string &insert(size_t pos, size_t n, char c) {
    return v.insert(pos, n, c);
  }
};


template<class T>
struct vector2D_view {
  std::vector<std::vector<T>> v;
  unsigned start_i, start_j;

public:
  vector2D_view(std::vector<std::vector<T>> view, unsigned __start_i=0, unsigned __start_j=0)
               : v{view}, start_i{__start_i}, start_j{__start_j}
               {}

  vector2D_view(const vector2D_view<T> &) = delete;
  vector2D_view(vector2D_view<T> &&) = delete;
  vector2D_view &operator=(const vector2D_view<T> &) = delete;
  vector2D_view &operator=(vector2D_view<T> &&) = delete;

  const vector_view<T> operator[](unsigned i) const {
    assert(start_i+i < v.size());
    return vector_view<T>(v[start_i+i], start_j);
  }

  vector_view<T> operator[](unsigned i) {
    assert(start_i+i < v.size());
    return vector_view<T>(v[start_i+i], start_j);
  }

  unsigned size() const {
    return v.size();
  }
};

template<>
struct vector2D_view<std::string> {
  std::vector<std::string> &v;
  unsigned start_i, start_j;

public:
  vector2D_view(std::vector<std::string> &view, unsigned __start_i=0, unsigned __start_j=0)
               : v{view}, start_i{__start_i}, start_j{__start_j}
               {}

  vector2D_view(const vector2D_view<std::string> &) = delete;
  vector2D_view(vector2D_view<std::string> &&) = delete;
  vector2D_view &operator=(const vector2D_view<std::string> &) = delete;
  vector2D_view &operator=(vector2D_view<std::string> &&) = delete;

  const vector_view<std::string> operator[](unsigned i) const {
    assert(start_i+i < v.size());
    return vector_view<std::string>(v[start_i+i], start_j);
  }

  vector_view<std::string> operator[](unsigned i) {
    assert(start_i+i < v.size());
    return vector_view<std::string>(v[start_i+i], start_j);
  }


  const vector_view<std::string> at_or(unsigned i, std::string s="") const {
    if(start_i+i < v.size()) {
      return this->operator[](i);
    } 
    return vector_view<std::string>{s, 0};
  }

  unsigned size() const {
    return v.size();
  }
};

using buffer_view = vector2D_view<std::string>;

#endif
