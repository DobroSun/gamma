#ifndef GAMMA_ARRAY_H
#define GAMMA_ARRAY_H

template<class T>
struct array {
  T *data = nullptr;
  unsigned capacity = 0;
  unsigned size = 0;

  static constexpr unsigned eight = 8;


  array() = default;
  array(unsigned cap) {
    assert(!data && !capacity && !size);
    capacity = cap;
    data = (T *)malloc(sizeof(T) * cap);
  }

  array(const array &other) = delete;
  array &operator=(const array &other) = delete;
  array(array &&other) = delete;
  array &operator=(array &&other) = delete;

  ~array() {
    free(data);
    data = nullptr;
    capacity = 0;
    size = 0;
  }

  T *add() {
    if(size == capacity) {
      reserve();
    }
    return &data[size++];
  }

  T *add(const T &val) {
    if(size == capacity) {
      reserve();
    }
    data[size] = val;
    return &data[size++];
  }

  T *add(T &&val) {
    if(size == capacity) {
      reserve();
    }
    data[size] = std::move(val);
    return &data[size++];
  }

  void reserve(unsigned new_cap=0) {
    if(!data) {
      assert(!capacity && !size && !data);

      new_cap = (new_cap)? new_cap: eight;

      data = (T*)malloc(sizeof(T) * new_cap);
      capacity = new_cap;

    } else {
      assert(data && capacity);

      new_cap = (new_cap)? new_cap: 2*capacity;

      if(new_cap > capacity) {
        auto new_data = (T*)malloc(sizeof(T) * new_cap);
        assert(new_data);
        if(size) {
          memcpy(new_data, data, sizeof(T) * size);
        }
        capacity = new_cap;

        free(data);
        data = new_data;

      } else {
        assert(new_cap <= capacity);
        if(size > new_cap) {
          size     = new_cap;
          capacity = new_cap;

          auto new_data = (T*)malloc(sizeof(T) * new_cap);
          assert(new_data);

          memcpy(new_data, data, sizeof(T) * size);

          free(data);
          data = new_data;

        } else {
          // Do nothing.
        }
      }
    }
  }

  void resize_with_no_init(unsigned new_size=0) {
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
};
#endif
