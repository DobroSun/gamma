#ifndef GAMMA_UTILITY_H
#define GAMMA_UTILITY_H

#ifndef NDEBUG
struct Location {
  const char *file;
  const char *function;
  u32 line;
  void *ptr;
  size_t allocated;
};

inline const u32 MAX_REPORTS       = 1024;
inline Location  error_reports[MAX_REPORTS] = {};
inline u32       num_reports = 0;

#define allocate(bytes) _allocate(bytes, __FILE__, __func__, __LINE__)
#define deallocate(ptr) _deallocate(ptr)

inline void* _allocate(size_t bytes, const char *file, const char *function, u32 line) {
  void *r = malloc(bytes);
  assert(r);
  memset(r, 0, bytes);

  assert(num_reports < MAX_REPORTS);
  Location loc;
  loc.file     = file;
  loc.function = function;
  loc.line     = line;
  loc.ptr      = r;
  loc.allocated = bytes;
  error_reports[num_reports++] = loc;

  return r;
}

inline void  _deallocate(void *ptr) {
  if(!ptr) return;

  bool found = false;
  u32 index;
  for(u32 i = 0; i < num_reports; i++) {
    Location it = error_reports[i];
    if(it.ptr == ptr) {
      found = true;
      index = i;
    }
  }
  if(found) {
    error_reports[index] = error_reports[--num_reports];
  }
  free(ptr);
}

inline void report_all_memory_leaks() {
  for(u32 i = 0; i < num_reports; i++) {
    Location it = error_reports[i];
    printf("%s:%u:%s \t::\twas allocated %lu bytes, but never freed!\n", it.file, it.line, it.function, it.allocated);
  }


  size_t total = 0;
  for(u32 i = 0; i < num_reports; i++) {
    total += error_reports[i].allocated;
  }
  if(total == 0) {
    printf("No memory leaks!\n");
  } else {
    printf("Total memory leaked: %lu\n", total);
  }
}
#else 
inline void* allocate(size_t bytes) {
  void *r = malloc(bytes);
  assert(r);
  memset(r, 0, bytes);
  return r;
}

inline void deallocate(void *ptr) {
  if(!ptr) return;
  return free(ptr);
}
#endif // NDEBUG

struct Junk {};
#define defer auto ANONYMOUS_NAME = Junk{} + [&]()
#define ANONYMOUS_NAME CONCAT(GAMMA, __LINE__)
#define CONCAT(A, B) CONCAT_IMPL(A, B)
#define CONCAT_IMPL(A, B) A##B

template<class F>
struct ScopeGuard {
  F fun;

  ScopeGuard(F &&f): fun{std::move(f)} {}
  ~ScopeGuard() { fun(); }
};

template<class F>
ScopeGuard<F> operator+(Junk, F &&fun) {
  return ScopeGuard<F>(std::move(fun));
}


struct literal {
  const char *data;
  size_t      size;

  char operator[](size_t i) const {
    assert(i < size);
    return data[i];
  }
};

template<size_t N>
inline literal to_literal(const char (&x)[N]) {
  literal l = {x, N-1};
  return l;
}

inline literal to_literal(const char *x, size_t N) {
  literal l = {x, N};
  return l;
}


inline bool operator==(char s, literal l) { return l.size == 1 && l.data[0] == s; }
inline bool operator==(literal l, char s) { return l.size == 1 && l.data[0] == s; }

inline bool operator==(const char *s, literal l) { return l.size && !strncmp(s, l.data, l.size); }
inline bool operator==(literal l, const char *s) { return l.size && !strncmp(s, l.data, l.size); }
inline bool operator!=(const char *s, literal l) { return !(s == l); }
inline bool operator!=(literal l, const char *s) { return !(s == l); }

inline bool operator==(literal l1, literal l2) {
  if(l1.size == l2.size) {
    return !strncmp(l1.data, l2.data, l1.size);
  } else {
    return false;
  }
}

inline bool operator!=(literal l1, literal l2) { return !(l1 == l2); }

inline std::ostream& operator<<(std::ostream &os, literal l) {
  for(size_t i = 0; i < l.size; i++) {
    os << l.data[i];
  }
  return os;
}

#define static_string_from_literal(name, l) \
  char name[l.size+1] = {}; \
  memcpy(name, l.data, l.size);

inline char *dynamic_string_from_literal(literal l) {
  char *r = (char *)allocate(l.size+1);
  memcpy(r, l.data, l.size);
  return r;
}

#define array_size(x) (sizeof((x)) / sizeof(*(x)))


struct Print {
  literal sep = to_literal(", "), end = to_literal("\n");

  void operator()() {
    std::cout << end;
    sep = to_literal(", "), end = to_literal("\n");
  }
  template<class T>
  void operator()(T &&last) {
    std::cout << std::boolalpha << std::forward<T>(last);
    operator()();
  }
  template<class T, class ...Args>
  void operator()(T&& first, Args&&... rest) {
    std::cout << std::boolalpha << std::forward<T>(first) << sep;
    operator()(std::forward<Args>(rest)...);
  }
};
static Print print;

struct Timer {
  std::chrono::_V2::steady_clock::time_point start;
  std::chrono::_V2::steady_clock::time_point end;

  Timer()  { start = std::chrono::steady_clock::now(); }
  ~Timer() { 
    end = std::chrono::steady_clock::now(); 
  
    f64 delta = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
    print.sep = to_literal("");
    if(delta < 1000) {
      print("ns: ", delta);
    } else if(delta >= 1000 && delta < 1000000) {
      print("us: ", delta/1000.);
    } else {
      print("ms: ", delta/1000000.);
    }
  }
};

#define measure_scope() Timer ANONYMOUS_NAME;
#define max(a, b) (((a) > (b)) ? (a) : (b))
#define min(a, b) (((a) < (b)) ? (a) : (b))
#define square(a) ((a)*(a))

#define For(array) size_t it_index = 0; for(auto it = (array).begin(); it != (array).end(); it++, it_index++)


template<class T, class U, size_t N>
bool one_of(const T c, const U (&x)[N]) {
  for(size_t i = 0; i < N; i++) {
    if(c == x[i]) { return true; }
  }
  return false;
}

template<class T, class F>
T* find_if(array<T> a, F f) {
  for(size_t i = 0; i < a.size; i++) {
    if(f(a[i])) {
      return &a[i];
    }
  }
  return NULL;
}





#endif
