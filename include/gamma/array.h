#ifndef GAMMA_ARRAY_H
#define GAMMA_ARRAY_H

#include "utility.h"


template<class T>
class array;


template<class T>
void copy(T *p1, T *p2, unsigned size) {
  for(unsigned i = 0; i < size; i++) {
    p2[i] = p1[i];
  }
}

template<class T>
void init(T *p, unsigned from, unsigned to) {
  for(unsigned i = from; i < to; i++) {
    ::new (&p[i]) T{};
  }
}

// @Note: string must use malloc.
template<class T>
void reserve_impl(unsigned size_to_alloc, T *&p, unsigned &__size, unsigned &__capacity, unsigned original_capacity) {
  assert(__size <= __capacity);
  if(!p) {
    assert(!__capacity);
    assert(!__size);

    __capacity = (!size_to_alloc)? original_capacity: size_to_alloc;
    p = new T[__capacity];
    assert(p);


  } else {
    assert(p);
    assert(__capacity);

    __capacity = (!size_to_alloc)? __capacity*2: size_to_alloc;
    auto new_p = new T[__capacity];
    assert(new_p);

    copy(p, new_p, __size);
    delete[] p;
    p = new_p;

  }
  assert(__size <= __capacity);
  assert(__capacity);
  assert(p);
}


// @MayLeak:
// resize on object that allocates in constructor will leak.
// Cause it calls constructor on new, and then inside init().
// But new T[__capacity] DO NOT constructs objects with default values(as it should).
// That is why I need that additional init() function call.
template<class T>
void resize_impl(unsigned size_to_resize, T *&p, unsigned &__size, unsigned &__capacity, unsigned original_capacity) {
  assert(__size <= __capacity);
  if(!p) {
    assert(!__capacity);
    assert(!__size);

    __capacity = (!size_to_resize)? original_capacity: size_to_resize;
    p = new T[__capacity];
    assert(p);
    init(p, 0, __capacity);
    __size = __capacity;

  } else {
    assert(__capacity);
    assert(__capacity < size_to_resize);
    assert(__size < size_to_resize);

    __capacity = (!size_to_resize)? __capacity*2: size_to_resize;
    auto new_p = new T[__capacity];
    assert(new_p);
    init(new_p, 0, __capacity);

    copy(p, new_p, __size);
    __size = __capacity;
    delete[] p;
    p = new_p;

  }
  assert(__capacity);
  assert(__size == __capacity);
  assert(p);
}

template<class T>
void resize_with_no_init_impl(unsigned size_to_resize, T *&p, unsigned &__size, unsigned &__capacity, unsigned original_capacity) {
  reserve_impl(size_to_resize, p, __size, __capacity, original_capacity);
  __size = __capacity;
}

template<class T>
void reserve_on_push_back_impl(T *&p, unsigned &__size, unsigned &__capacity, unsigned original_capacity) {
  if(__size == __capacity) {
    reserve_impl(0, p, __size, __capacity, original_capacity);
  }
}

template<class T>
void push_back_impl(const T &val, T *&p, unsigned &__size, unsigned &__capacity, unsigned original_capacity) {
  reserve_on_push_back_impl(p, __size, __capacity, original_capacity);
  p[__size++] = val;
}

template<class T>
void push_back_impl(T &&val, T *&p, unsigned &__size, unsigned &__capacity, unsigned original_capacity) {
  reserve_on_push_back_impl(p, __size, __capacity, original_capacity);
  p[__size++] = std::move(val);
}

template<class T>
void pop_back_impl(T *&p, unsigned &__size) {
  assert(__size > 0);
  p[--__size] = '\0';
}

template<class T>
T &front_impl(T *p, unsigned &__size) {
  assert(__size > 0);
  return p[0];
}

template<class T>
T &front_impl(T *p, const unsigned &__size) {
  assert(__size > 0);
  return p[0];
}

template<class T>
T& back_impl(T *p, unsigned &__size) {
  assert(__size > 0);
  return p[__size-1];
}

template<class T>
T& back_impl(T *p, const unsigned &__size) {
  assert(__size > 0);
  return p[__size-1];
}

void inline clear_impl(unsigned &__size) {
  __size = 0;
}

template<class T>
void swap_impl(array<T> &other, T *&p, unsigned &__size, unsigned &__capacity) {
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


unsigned inline size_impl(unsigned __size) {
  return __size;
}

bool inline empty_impl(unsigned __size) {
  return __size == 0;
}

unsigned inline capacity_impl(unsigned __capacity) {
  return __capacity;
}

template<class T>
T &at_impl(unsigned i, T *p, unsigned &__size) {
  assert(i >= 0 && i < __size);
  return p[i];
}

template<class T>
T &at_impl(unsigned i, T *p, const unsigned &__size) {
  assert(i >= 0 && i < __size);
  return p[i];
}



template<class T>
void on_copy_assign_impl(const array<T> &other, T *&p, unsigned &__size, unsigned &__capacity, unsigned original_capacity) {
  auto other_size = other.__size;
  if(other_size > __capacity) {
    resize_with_no_init_impl(other_size, p, __size, __capacity, original_capacity);
  } else {
    __size = other_size;
    __capacity = other_size;
  }

  for(unsigned i = 0; i < other_size; i++) {
    p[i] = other.p[i];
  }
}

template<class T>
void on_move_assign_impl(array<T> &&other, T *&p, unsigned &__size, unsigned &__capacity) {
  __size = other.__size;
  __capacity = other.__capacity;
  p = std::move(other.p);

  other.__size = 0;
  other.__capacity = 0;
  other.p = nullptr;
}



template<class T>
struct array {
  T *p = nullptr;
  unsigned __capacity = 0;
  unsigned __size = 0;
  constexpr static unsigned original_capacity = 8;


  array() = default;

  array(const array &other) {
    on_copy_assign_impl(other, p, __size, __capacity, original_capacity);
  }

  array &operator=(const array &other) {
    on_copy_assign_impl(other, p, __size, __capacity, original_capacity);
    return *this;
  }

  array(array &&other) {
    on_move_assign_impl(std::move(other), p, __size, __capacity);
  }

  array &operator=(array &&other) {
    on_move_assign_impl(std::move(other), p, __size, __capacity);
    return *this;
  }

  ~array() {
    delete[] p;
  }

  void reserve(unsigned size_to_reserve=0) {
    reserve_impl(size_to_reserve, p, __size, __capacity, original_capacity);
  }

  void resize(unsigned size_to_resize=0) {
    resize_impl(size_to_resize, p, __size, __capacity, original_capacity);
  }

  void resize_with_no_init(unsigned size_to_resize=0) {
    resize_with_no_init_impl(size_to_resize, p, __size, __capacity, original_capacity);
  }

  void push_back(const T &val) {
    push_back_impl(val, p, __size, __capacity, original_capacity);
  }

  void push_back(T &&val) {
    push_back_impl(std::move(val), p, __size, __capacity, original_capacity);
  }

  void pop_back() {
    pop_back_impl(p, __size);
  }

  T& front() {
    return front_impl(p, __size);
  }

  const T& front() const {
    return front_impl(p, __size);
  }

  T& back() {
    return back_impl(p, __size);
  }

  const T& back() const {
    return back_impl(p, __size);
  }

  void clear() {
    clear_impl(__size);
  }

  void swap(array &other) {
    swap_impl(other, p, __size, __capacity);
  }

  unsigned size() const {
    return size_impl(__size);
  }

  bool empty() const {
    return empty_impl(__size);
  }

  unsigned capacity() const {
    return capacity_impl(__capacity);
  }

  unsigned maxsize() const {
    return -1;
  }

  T &operator[](unsigned i) {
    return at_impl(i, p, __size);
  }

  const T &operator[](unsigned i) const {
    return at_impl(i, p, __size);
  }

  void reserve_on_push_back() {
    reserve_on_push_back_impl(p, __size, __capacity, original_capacity);
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


template<>
struct array<char> {
  char *p = nullptr;
  unsigned __capacity = 0;
  unsigned __size = 0;
  constexpr static unsigned original_capacity = 8;


  array() = default;
  array(char c) {
    reserve(original_capacity);
    p[__size++] = c;
  }
  array(const char *c) {
    for_each(c) {
      push_back(*it);
    }
  }

  array(const array &other) {
    on_copy_assign_impl(other, p, __size, __capacity, original_capacity);
  }

  array &operator=(const array &other) {
    on_copy_assign_impl(other, p, __size, __capacity, original_capacity);
    return *this;
  }

  array(array &&other) {
    on_move_assign_impl(std::move(other), p, __size, __capacity);
  }

  array &operator=(array &&other) {
    on_move_assign_impl(std::move(other), p, __size, __capacity);
    return *this;
  }

  ~array() {
    delete[] p;
  }

  void reserve(unsigned size_to_reserve=0) {
    reserve_impl(size_to_reserve, p, __size, __capacity, original_capacity);
  }

  void resize(unsigned size_to_resize=0) {
    resize_impl(size_to_resize, p, __size, __capacity, original_capacity);
  }

  void resize_with_no_init(unsigned size_to_resize=0) {
    resize_with_no_init_impl(size_to_resize, p, __size, __capacity, original_capacity);
  }

  void push_back(const char &val) {
    push_back_impl(val, p, __size, __capacity, original_capacity);
  }

  void push_back(char &&val) {
    push_back_impl(std::move(val), p, __size, __capacity, original_capacity);
  }

  void pop_back() {
    pop_back_impl(p, __size);
  }

  char& front() {
    return front_impl(p, __size);
  }

  const char& front() const {
    return front_impl(p, __size);
  }

  char& back() {
    return back_impl(p, __size);
  }

  const char& back() const {
    return back_impl(p, __size);
  }

  void clear() {
    clear_impl(__size);
  }

  void swap(array &other) {
    swap_impl(other, p, __size, __capacity);
  }

  unsigned size() const {
    return size_impl(__size);
  }

  bool empty() const {
    return empty_impl(__size);
  }

  unsigned capacity() const {
    return capacity_impl(__capacity);
  }

  unsigned maxsize() const {
    return -1;
  }

  char &operator[](unsigned i) {
    return at_impl(i, p, __size);
  }

  const char &operator[](unsigned i) const {
    return at_impl(i, p, __size);
  }

  void reserve_on_push_back() {
    reserve_on_push_back_impl(p, __size, __capacity, original_capacity);
  }

  char *c_str() const {
    p[__size] = '\0'; // @Wrong: Invalid write if __size == __capacity
    return p;
  }

  char *data() const {
    p[__size] = '\0'; // @Wrong: Invalid write if __size == __capacity
    return p;
  }
};

#endif
