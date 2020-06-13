#ifndef GAMMA_ARRAY_H
#define GAMMA_ARRAY_H


template<class T>
void copy(T *p1, T *p2, unsigned size) {
  for(unsigned i = 0; i < size; i++) {
    p2[i] = p1[i];
  }
}

template<class T>
void init(T *p, unsigned from, unsigned to) {
  auto d = T{};
  for(unsigned i = from; i < to; i++) {
    p[i] = d;
  }
}

template<class T>
class array {
  T *p;
  unsigned __capacity; 
  unsigned __size;

  constexpr static unsigned original_capacity = 8;


public:
  array(): p{nullptr}, __capacity{0}, __size{0} {
  }

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


  void reserve(unsigned size_to_alloc=0) {
    if(!p) {
      assert(!__capacity);
      assert(!__size);
      __capacity = (!size_to_alloc)? original_capacity: size_to_alloc;
      p = (T*)malloc(sizeof(T) * __capacity);

    } else {
      assert(__capacity);

      __capacity = (!size_to_alloc)? __capacity*2: size_to_alloc;
      auto new_p = (T*)malloc(sizeof(T) * __capacity);

      copy(p, new_p, __size);
      free(p);
      p = new_p;
    }
  }

  void resize(unsigned size_to_resize=0) {
    if(!p) {
      assert(!__capacity);
      assert(!__size);

      __capacity = (!size_to_resize)? original_capacity: size_to_resize;
      __size = __capacity;
      p = (T*)malloc(sizeof(T) * __capacity);
      init(p, 0, __size);


    } else {
      assert(__capacity);
      assert(__capacity < size_to_resize);
      assert(__size < size_to_resize);

      __capacity = (!size_to_resize)? __capacity*2: size_to_resize;
      auto new_p = (T*)malloc(sizeof(T) * __capacity);

      copy(p, new_p, __size);
      init(new_p, __size, __capacity); // from old_size to new_size.
      __size = __capacity;
      free(p);
      p = new_p;
    }
  }

  void push_back(const T &val) {
    if(__size+1 > __capacity) {
      reserve();
    }
    p[__size++] = val;
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
    assert(__size > 0);
    return p[0];
  }

  T& back() {
    assert(__size > 0);
    return p[__size-1];
  }

  const T& back() const {
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
    assert(i >= 0 && i < __size);
    return p[i];
  }

private:
  void on_copy_assign(const array &other) {
    auto other_size = other.__size;
    while(other_size > __capacity) {
      reserve();
    }
    for(unsigned i = 0; i < other_size; i++) {
      push_back(other.p[i]);
    }
  }

  void on_move_assign(array &&other) {
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


// array vs std::vector.
// This impl beats std::vector on operations:
// 1) push_back; 
// 2) reserve+push_back;
//      @Way faster on small types(int, double, etc).
//      and little bit faster on big types.

// Slower on: 
// 1) resize+push_back; // for small types, and pretty equally on big.

#endif
