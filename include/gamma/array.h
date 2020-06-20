#ifndef GAMMA_ARRAY_H
#define GAMMA_ARRAY_H

#include "gamma/utility.h"

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


  } else {
    assert(p);
    assert(__capacity);

    __capacity = (!size_to_alloc)? __capacity*2: size_to_alloc;
    auto new_p = new T[__capacity];

    copy(p, new_p, __size);
    delete[] p;
    p = new_p;

  }
  assert(__size <= __capacity);
  assert(__capacity);
  assert(p);
}


template<class T>
void resize_impl(unsigned size_to_resize, T *&p, unsigned &__size, unsigned &__capacity, unsigned original_capacity) {
  assert(__size <= __capacity);
  if(!p) {
    assert(!__capacity);
    assert(!__size);

    __capacity = (!size_to_resize)? original_capacity: size_to_resize;
    p = new T[__capacity];
    init(p, 0, __capacity);
    __size = __capacity;

  } else {
    assert(__capacity);
    assert(__capacity < size_to_resize);
    assert(__size < size_to_resize);

    __capacity = (!size_to_resize)? __capacity*2: size_to_resize;
    auto new_p = new T[__capacity];
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
  if(__size+1 > __capacity) {
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

void inline pop_back_impl(unsigned &__size) {
  assert(__size > 0);
  --__size;
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
    pop_back_impl(__size);
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


template<>
struct array<char> {
  char *p = nullptr;
  unsigned __capacity = 0;
  unsigned __size = 0;
  constexpr static unsigned original_capacity = 8;


  array() = default;
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
    pop_back_impl(__size);
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

};

using string = array<char>;




// array vs std::vector.
// This impl beats std::vector on operations:
// 1) push_back; 
// 2) reserve+push_back;
//      @Way faster on small types(int, double, etc).
//      and little bit faster on big types.

// Slower on: 
// 1) resize+push_back; // for small types, and pretty equally on big.

/*
template<>
class array<char> {
  char *p = nullptr;
  unsigned __capacity = 0;
  unsigned __size = 0;
  bool __junk;

  constexpr static unsigned original_capacity = 24;
  constexpr static unsigned npos = -1;
    

public:
  array() = default;
  array(const char *c) {
    unsigned c_size = strlen(c);
    auto bytes = (unsigned char *)(this);
    auto size_on_stack = sizeof(*this);

    assert(original_capacity == size_on_stack);
    auto last_byte_index = size_on_stack-1;
    if(c_size < last_byte_index) { 
      // Via SSO string can store 22 characters, + 1 size of string +
      // 1 flag indicating whether this string is short or not.

      bytes[last_byte_index] = 1; // string is short - SSO.
      bytes[size_on_stack-2] = c_size;

      for(unsigned i = 0; i < c_size; i++) {
        bytes[i] = c[i];
      }

    } else {
      assert(!p);
      assert(!__size);
      assert(!__capacity);
      bytes[last_byte_index] = 0; // string is long - no SSO.

      if(__capacity < c_size) {
        reserve_long(c_size+1); // + 1 for null terminator.
      }
      __size = c_size;

      for(unsigned i = 0; i < c_size; i++) {
        p[i] = c[i];
      }
    }
  }
  ~array() {
    auto bytes = (unsigned char *)(this);
    auto size_on_stack = sizeof(*this);
    auto is_short = bytes[size_on_stack-1];

    if(is_short) {
      // Nothing to do.
    } else {
      free(p);
    }
  }

  void reserve(unsigned size_to_reserve=0) {
    auto bytes = (unsigned char *)(this);
    auto size_on_stack = sizeof(*this);
    auto is_short = bytes[size_on_stack-1];

    if(is_short) {
      reserve_short(size_to_reserve);

    } else {
      reserve_long(size_to_reserve);
    }
  }

  void reserve_short(unsigned size_to_reserve=0) {
    auto bytes = (unsigned char *)(this);
    auto size_on_stack = sizeof(*this);
    auto last_byte_index = size_on_stack-1;
    auto &is_short = bytes[last_byte_index];

    assert(is_short == 1);

    auto last_character_index = size_on_stack-2;
    auto &new_size = bytes[last_character_index];
    auto new_capacity = (!size_to_reserve)? new_size*2: size_to_reserve;
    assert(new_size < new_capacity);

    if(new_capacity < last_byte_index) {
      // We are still in stack size's limit.

      // Reserve doesn't change the size of string.
      // it shall only allocate new space.
      // and if it's new_capacity still < then current_capacity(sizeof(string)-2).
      // we have nothing to do.

    } else {
      // Need to allocate new string.

      auto new_p = (char *)malloc(sizeof(char) * new_capacity);
      for(unsigned i = 0; i < last_character_index; i++) {
        new_p[i] = bytes[i];
      }

      is_short = 0;
      __size = new_size;
      __capacity = new_capacity;
      p = new_p;

      assert(__size < __capacity);
    }
  }

  void reserve_long(unsigned size_to_reserve=0) {
    {
      auto bytes = (unsigned char *)(this);
      auto size_on_stack = sizeof(*this);
      auto is_short = bytes[size_on_stack-1];
      assert(is_short == 0);
    }

    // Copy&Paste: from array.
    if(!p) {
      assert(!__capacity);
      assert(!__size);
      __capacity = (!size_to_reserve)? original_capacity: size_to_reserve;
      p = (char*)malloc(sizeof(char) * __capacity);

    } else {
      assert(__capacity);
      assert(p);

      __capacity = (!size_to_reserve)? __capacity*2: size_to_reserve;
      auto new_p = (char*)malloc(sizeof(char) * __capacity);

      copy(p, new_p, __size);
      free(p);
      p = new_p;
    }
    assert(__size < __capacity);
  }

  void resize(unsigned size_to_resize=0) {
    auto bytes = (unsigned char *)(this);
    auto size_on_stack = sizeof(*this);
    auto last_byte_index = size_on_stack-1;
    auto is_short = bytes[last_byte_index];

    if(is_short) {
      auto last_character_index = size_on_stack-2;
      auto &new_size = bytes[last_character_index];
      auto new_capacity = (!size_to_resize)? new_size*2: size_to_resize;
      assert(new_size < new_capacity);

      if(new_capacity < last_byte_index) {
        init(bytes, new_size, last_character_index);
        new_size = new_capacity;

      } else {
        
        auto new_p = (char *)malloc(sizeof(char) * new_capacity);
        for(unsigned i = 0; i < last_character_index; i++) {
          new_p[i] = bytes[i];
        }
        init(bytes, new_size, new_capacity);

        is_short = 0;
        __size = new_capacity;
        __capacity = new_capacity;
        p = new_p;

        assert(__size < __capacity);
      }

    } else {
      // Copy&Paste: from array.
      if(!p) {
        assert(!__capacity);
        assert(!__size);

        __capacity = (!size_to_resize)? original_capacity: size_to_resize;
        __size = __capacity;
        p = (char*)malloc(sizeof(char) * __capacity);
        init(p, 0, __size);

        assert(__size < __capacity);

      } else {
        assert(__capacity);
        assert(__capacity < size_to_resize);
        assert(__size <= __capacity);

        __capacity = (!size_to_resize)? __capacity*2: size_to_resize;
        auto new_p = (char*)malloc(sizeof(char) * __capacity);

        copy(p, new_p, __size);
        init(new_p, __size, __capacity); // from old_size to new_size.
        __size = __capacity;
        free(p);
        p = new_p;

        assert(__size < __capacity);
      }
    }
  }

  unsigned size() const {
    auto bytes = (unsigned char *)(this);
    auto size_on_stack = sizeof(*this);
    auto is_short = bytes[size_on_stack-1];
    
    if(is_short) {
      return bytes[size_on_stack-2];
    } else {
      return __size;
    }
  }

  unsigned capacity() const {
    auto bytes = (unsigned char *)(this);
    auto size_on_stack = sizeof(*this);
    auto is_short = bytes[size_on_stack-1];
    
    if(is_short) {
      return size_on_stack-2;
    } else {
      return __capacity;
    }

  }

  char *data() {
    auto bytes = (unsigned char *)(this);
    auto size_on_stack = sizeof(*this);
    auto is_short = bytes[size_on_stack-1];
    
    if(is_short) {
      auto size = bytes[size_on_stack-2];
      bytes[size] = '\0';
      return (char*)bytes;

    } else {
      p[__size] = '\0';
      return p;
    }
  }

  const char *data() const {
    auto bytes = (unsigned char *)(this);
    auto size_on_stack = sizeof(*this);
    auto is_short = bytes[size_on_stack-1];
    
    if(is_short) {
      auto size = bytes[size_on_stack-2];
      bytes[size] = '\0';
      return (const char*)bytes;

    } else {
      p[__size] = '\0';
      return p;
    }
  }

  char *c_str() {
    // Copy&Paste: same as data().
    auto bytes = (unsigned char *)(this);
    auto size_on_stack = sizeof(*this);
    auto is_short = bytes[size_on_stack-1];
    
    if(is_short) {
      auto size = bytes[size_on_stack-2];
      bytes[size] = '\0';
      return (char*)bytes;

    } else {
      p[__size] = '\0';
      return p;
    }
  }

  const char *c_str() const {
    auto bytes = (unsigned char *)(this);
    auto size_on_stack = sizeof(*this);
    auto is_short = bytes[size_on_stack-1];
    
    if(is_short) {
      auto size = bytes[size_on_stack-2];
      bytes[size] = '\0';
      return (const char*)bytes;

    } else {
      p[__size] = '\0';
      return p;
    }
  }

  char &operator[](unsigned i) {
    auto bytes = (unsigned char *)(this);
    auto size_on_stack = sizeof(*this);
    auto is_short = bytes[size_on_stack-1];

    if(is_short) {
      assert(i >= 0 && i < size_on_stack-2);
      return (char&)bytes[i];
    } else {
      assert(i >= 0 && i < __size);
      return p[i];
    }
  }

  const char &operator[](unsigned i) const {
    auto bytes = (unsigned char *)(this);
    auto size_on_stack = sizeof(*this);
    auto is_short = bytes[size_on_stack-1];

    if(is_short) {
      assert(i >= 0 && i < size_on_stack-2);
      return (char&)bytes[i];
    } else {
      assert(i >= 0 && i < __size);
      return p[i];
    }
  }

  bool is_short() const {
    return (((unsigned char*)(this))[sizeof(*this)-1]) == 1;
  }
};
*/



#endif
