#ifndef GAMMA_ARRAY_H
#define GAMMA_ARRAY_H

template<class T>
struct dyn_array {
  T *data = nullptr;
  unsigned capacity = 0;
  unsigned size = 0;

  static constexpr unsigned eight = 8;


  dyn_array() = default;
  dyn_array(unsigned cap) {
    init(cap);
  }

  dyn_array(const dyn_array &other) {
    on_copy_construct(other);
  }

  dyn_array(dyn_array &&other) {
    on_move_assign(std::move(other));
  }

  dyn_array &operator=(const dyn_array &other) {
    on_copy_assign(other);
    return *this;
  }

  dyn_array &operator=(dyn_array &&other) {
    on_move_assign(std::move(other));
    return *this;
  }

  ~dyn_array() {
    delete[] data;
    data = nullptr;
    capacity = 0;
    size = 0;
  }

  void init(unsigned cap) {
    assert(!data && !capacity && !size);
    capacity = cap;
    data = new T[cap];
  }

  T &add(const T &val) {
    if(size == capacity) {
      reserve();
    }
    data[size] = val;
    return data[size++];
  }

  T &add(T &&val) {
    if(size == capacity) {
      reserve();
    }
    data[size] = std::move(val);
    return data[size++];
  }

  void reserve(unsigned new_cap=0) {
    if(!data) {
      assert(!capacity && !size && !data);

      new_cap = (new_cap)? new_cap: eight;

      data = new T[new_cap];
      capacity = new_cap;

    } else {
      assert(data && capacity);

      new_cap = (new_cap)? new_cap: 2*capacity;

      if(new_cap > capacity) {
        auto new_data = new T[new_cap];
        assert(new_data);
        if(size) {
          memcpy(new_data, data, sizeof(T) * size);
        }
        capacity = new_cap;

        delete[] data;
        data = new_data;

      } else {
        assert(new_cap <= capacity);
        if(size > new_cap) {
          size     = new_cap;
          capacity = new_cap;

          auto new_data = new T[new_cap];
          assert(new_data);

          memcpy(new_data, data, sizeof(T) * size);

          delete[] data;
          data = new_data;

        } else {
          // Do nothing.
        }
      }
    }
  }

  void resize(unsigned new_size=0) {
    reserve(new_size);
    size = capacity;
  }

  T &operator[](unsigned index) {
    assert(index < size);
    return data[index];
  }

  const T &operator[](unsigned index) const {
    assert(index < size);
    return data[index];
  }

  bool empty() const {
    return size == 0;
  }

private:
  void on_copy_construct(const dyn_array &other) {
    assert(!data && !capacity && !size);
    resize(other.size);
  }

  void on_copy_assign(const dyn_array &other) {
    resize(other.size);
  }

  void on_move_assign(dyn_array &&other) {
    data = other.data;
    capacity = other.capacity;
    size = other.size;

    other.data = nullptr;
    other.capacity = 0;
    other.size = 0;
  }
};


template<class T>
bool operator==(const dyn_array<T> &a, const dyn_array<T> &b) {
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

inline char *to_string(const dyn_array<char> &a) {
  char *ret = (char*)malloc(sizeof(char) * (a.size+1));

  memcpy(ret, a.data, sizeof(char) * a.size);
  ret[a.size] = '\0';

  return ret;
}


#endif
