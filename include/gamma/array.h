#ifndef GAMMA_ARRAY_H
#define GAMMA_ARRAY_H

#include "utility.h"


template<class T>
class array;


template<class T>
void copy(T *from, T *to, unsigned size) {
  for(unsigned i = 0; i < size; i++) {
    to[i] = from[i];
  }
}

template<class T>
void init(T *p, unsigned from, unsigned to) {
  for(unsigned i = from; i < to; i++) {
    new (&p[i]) T{};
  }
}


template<class T>
class array {
public:
  T *p = nullptr;
  unsigned __capacity = 0;
  unsigned __size = 0;
  constexpr static unsigned original_capacity = 8;


  array() = default;
  array(const array &other) {
    on_copy_assign(other);
  }

  array &operator=(const array &other) {
    on_copy_assign(other);
    return *this;
  }

  array(array &&other) {
    on_move_assign(std::move(other));
  }

  array &operator=(array &&other) {
    on_move_assign(std::move(other));
    return *this;
  }

  ~array() {
    free(p);
  }

  void reserve(unsigned size_to_reserve=0) {
    assert(__size <= __capacity);
    if(!p) {
      assert(!__capacity);
      assert(!__size);

      __capacity = (!size_to_reserve)? original_capacity: size_to_reserve;
      p = (T*)malloc(sizeof(T) * __capacity);
      assert(p);


    } else {
      assert(p);
      assert(__capacity);

      __capacity = (!size_to_reserve)? __capacity*2: size_to_reserve;
      auto new_p = (T*)malloc(sizeof(T) * __capacity);
      assert(new_p);

      copy(p, new_p, __size);
      free(p);
      p = new_p;

    }
    assert(__size <= __capacity);
    assert(__capacity);
    assert(p);
  }

  void resize(unsigned size_to_resize=0) {
    assert(__size <= __capacity);
    if(!p) {
      assert(!__capacity);
      assert(!__size);

      __capacity = (!size_to_resize)? original_capacity: size_to_resize;
      p = (T*)malloc(sizeof(T) * __capacity);
      assert(p);
      init(p, 0, __capacity);
      __size = __capacity;

    } else {
      assert(__capacity);
      assert(__capacity < size_to_resize);
      assert(__size < size_to_resize);

      __capacity = (!size_to_resize)? __capacity*2: size_to_resize;
      auto new_p = (T*)malloc(sizeof(T) * __capacity);
      assert(new_p);
      init(new_p, __size, __capacity);

      copy(p, new_p, __size);
      __size = __capacity;
      free(p);
      p = new_p;

    }
    assert(__capacity);
    assert(__size == __capacity);
    assert(p);
  }

  void resize_with_no_init(unsigned size_to_resize=0) {
    reserve(size_to_resize);
    __size = __capacity;
  }

  void push_back(const T &val) {
    reserve_on_push_back();
    p[__size++] = val;
  }

  void push_back(T &&val) {
    // Copy&Paste.
    reserve_on_push_back();
    p[__size++] = val;
  }

  void push_back_no_check(const T &val) {
    assert(__size < __capacity);
    p[__size++] = val;
  }

  void push_back_no_check(T &&val) {
    // Copy&Paste.
    assert(__size < __capacity);
    p[__size++] = std::move(val);
  }

  void pop_back() {
    assert(__size > 0);
    --__size;
  }

  T& front() {
    assert(__size > 0);
    return p[0];
  }

  const T& front() const {
    // Copy&Paste.
    assert(__size > 0);
    return p[0];
  }

  T& back() {
    assert(__size > 0);
    return p[__size-1];
  }

  const T& back() const {
    // Copy&Paste.
    assert(__size > 0);
    return p[__size-1];
  }

  void clear() {
    __size = 0;
  }

  void swap(array &other) {
    auto tmp_capacity = other.__capacity;
    auto tmp_size = other.__size;
    auto *tmp_p = other.p;

    other.__capacity = __capacity;
    other.__size = __size;
    other.p = p;

    __capacity = tmp_capacity;
    __size = tmp_size;
    p = tmp_p;
  }

  unsigned size() const {
    return __size;
  }

  bool empty() const {
    return __size == 0;
  }

  unsigned capacity() const {
    return __capacity;
  }

  unsigned maxsize() const {
    return -1;
  }

  T &operator[](unsigned i) {
    assert(i >= 0 && i < __size);
    return p[i];
  }

  const T &operator[](unsigned i) const {
    // Copy&Paste.
    assert(i >= 0 && i < __size);
    return p[i];
  }

  void reserve_on_push_back() {
    if(__size == __capacity) {
      reserve();
    }
  }

  T *data() {
    return p;
  }

  const T *data() const {
    return p;
  }

private:
  void on_copy_assign(const array<T> &other) {
    auto other_size = other.__size;
    if(other_size > __capacity) {
      resize_with_no_init(other_size);
    } else {
      __size = other_size;
      __capacity = other_size;
    }

    copy(other.p, p, other_size);
  }

  void on_move_assign(array<T> &&other) {
    __size = other.__size;
    __capacity = other.__capacity;
    p = std::move(other.p);

    other.__size = 0;
    other.__capacity = 0;
    other.p = nullptr;
  }
};

template<class T>
bool operator==(const array<T> &a, const array<T> &b) {
  auto a_size = a.size();
  if(a_size != b.size()) {
    return false;
  }
  for(unsigned i = 0; i < a_size; i++) {
    if(a[i] != b[i]) {
      return false;
    }
  }
  return true;
}

template<class T>
array<T> operator+(const array<T> &a, const array<T> &b) {
  array<T> ret;
  auto b_size = b.size();
  auto a_size = a.size();

  ret.resize_with_no_init(a_size + b_size);

  unsigned count = 0;
  for(unsigned i = 0; i < a_size; i++) {
    ret[count] = a[i];
    count++;
  }
  for(unsigned i = 0; i < b_size; i++) {
    ret[count] = b[i];
    count++;
  }
  return ret;
}


class string: public array<char> {
public:
  string() = default;
  string(char c) {
    reserve();
    push_back_no_check(c);
  }

  string(const char *c) {
    reserve();
    for_each(c) {
      push_back(*it);
    }
  }

  char *c_str() {
    p[__size] = '\0';
    return p;
  }

  const char *c_str() const {
    p[__size] = '\0';
    return p;
  }

  char *data() {
    return c_str();
  }

  const char *data() const {
    return c_str();
  }
};

inline string operator+(const string &a, const string &b) {
  string ret;
  auto b_size = b.size();
  auto a_size = a.size();

  ret.reserve(a_size + b_size + 1);

  for(unsigned i = 0; i < a_size; i++) {
    ret.push_back_no_check(a[i]);
  }
  for(unsigned i = 0; i < b_size; i++) {
    ret.push_back_no_check(b[i]);
  }
  return ret;
}

#endif
