#ifndef GAMMA_VIEW_H
#define GAMMA_VIEW_H



template<class T>
struct vector_view {
  const std::vector<T> &v;
  unsigned start;

public:
  vector_view(const std::vector<T> &view, unsigned __start=0): v{view}, start{__start} {}

  vector_view(const vector_view<T> &) = delete;
  vector_view(vector_view<T> &&) = delete;
  vector_view &operator=(const vector_view<T> &) = delete;
  vector_view &operator=(vector_view<T> &&) = delete;

  const T &operator[](unsigned i) const {
    return v[start+i];
  }
  int size() const {
    return v.size();
  }
};

template<>
struct vector_view<std::string> {
  const std::string &v;
  unsigned start;

public:
  vector_view(const std::string &view, unsigned __start=0): v{view}, start{__start} {}

  vector_view(const std::string &) = delete;
  vector_view(std::string &&) = delete;
  vector_view &operator=(const std::string &) = delete;
  vector_view &operator=(std::string &&) = delete;

  const char &operator[](unsigned i) const {
    return v[start+i];
  }
  int size() const {
    return v.size();
  }
};


template<class T>
struct vector2D_view {
  const std::vector<std::vector<T>> &v;
  unsigned start_i, start_j;

public:
  vector2D_view(const std::vector<std::vector<T>> &view, unsigned __start_i=0, unsigned __start_j=0)
               : v{view}, start_i{__start_i}, start_j{__start_j}
               {}

  vector2D_view(const vector2D_view<T> &) = delete;
  vector2D_view(vector2D_view<T> &&) = delete;
  vector2D_view &operator=(const vector2D_view<T> &) = delete;
  vector2D_view &operator=(vector2D_view<T> &&) = delete;

  const vector_view<T> operator[](unsigned i) const {
    return vector_view<T>(v[start_i+i], start_j);
  }
  unsigned size() const {
    return v.size();
  }
};

template<>
struct vector2D_view<std::string> {
  const std::vector<std::string> &v;
  unsigned start_i, start_j;

public:
  vector2D_view(const std::vector<std::string> &view, unsigned __start_i=0, unsigned __start_j=0)
               : v{view}, start_i{__start_i}, start_j{__start_j}
               {}

  vector2D_view(const vector2D_view<std::string> &) = delete;
  vector2D_view(vector2D_view<std::string> &&) = delete;
  vector2D_view &operator=(const vector2D_view<std::string> &) = delete;
  vector2D_view &operator=(vector2D_view<std::string> &&) = delete;

  const vector_view<std::string> operator[](unsigned i) const {
    return vector_view<std::string>(v[start_i+i], start_j);
  }
  unsigned size() const {
    return v.size();
  }

};

using textures_view = vector_view<SDL_Texture *>;
using buffer_view = vector2D_view<std::string>;

#endif
