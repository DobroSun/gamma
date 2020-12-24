#ifndef GAMMA_UTILITY_H
#define GAMMA_UTILITY_H

inline char *concat(const char *a, const char *b) {
  char *ret;
  const size_t s1 = strlen(a);
  const size_t s2 = strlen(b);

  ret = (char *)malloc(sizeof(char) * (s1 + s2 + 1));
  assert(ret);

  memcpy(ret, a, sizeof(char) * s1);
  memcpy(ret + s1, b, sizeof(char) * s2);
  ret[s1+s2] = '\0';

  return ret;
}

struct Junk {};
#define defer auto ANONYMOUS_NAME = Junk{} + [&]()
#define ANONYMOUS_NAME CONCAT(GAMMA, __LINE__)
#define CONCAT(A, B) CONCAT_IMPL(A, B)
#define CONCAT_IMPL(A, B) A##B

template<class F>
class ScopeGuard {
  F fun;

public:
  ScopeGuard(F &&f): fun{std::move(f)} {}
  ~ScopeGuard() {
      fun();
  }
};

template<class F>
ScopeGuard<F> operator+(Junk, F &&fun) {
  return ScopeGuard<F>(std::move(fun));
}


void inline print() {
  std::cout << std::endl;
}

template<class T, class ...Args>
void print(T&& first, Args&&... rest) {
  std::cout << std::boolalpha << std::forward<T>(first);
  print(std::forward<Args>(rest)...);
}

#define sleep(x) usleep(x)
#define for_each(c_array) for(auto it = c_array; *it != '\0'; it++)

struct literal {
  const char *data;
  size_t      size;

  literal() = default;
  template<size_t N>
  literal(const char (&x)[N]) {
    data = x;
    size = N-1;
  }
  literal(const char *x, size_t N) {
    data = x;
    size = N;
  }
  literal(const literal &o) {
    data = o.data;
    size = o.size;
  }
  literal &operator=(const literal &o) {
    data = o.data;
    size = o.size;
    return *this;
  }
  literal(literal &&o) {
    data = std::move(o.data);
    size = std::move(o.size);
  }
  literal &operator=(literal &&o) {
    data = std::move(o.data);
    size = std::move(o.size);
    return *this;
  }
};

inline bool operator==(const char *s, const literal &l) {
  return !strncmp(s, l.data, l.size);
}

inline bool operator==(const literal &l, const char *s) {
  return !strncmp(s, l.data, l.size);
}

inline bool operator==(const literal &l1, const literal &l2) {
  if(l1.size == l2.size) {
    return !strncmp(l1.data, l2.data, l1.size);
  } else {
    return false;
  }
}


#define to_literal(a) literal{a.data, a.size}
inline bool operator==(const literal &l, const string_t &s) {
  return to_literal(s) == l;
}
inline bool operator==(const string_t &s, const literal &l) {
  return to_literal(s) == l;
}

inline std::ostream& operator<<(std::ostream &os, const literal &l) {
  for(size_t i = 0; i < l.size; i++) {
    os << l.data[i];
  }
  return os;
}

#define get_string_from_literal(name, l) \
  char name[l.size+1]; \
  { \
    for(decltype(l.size) i = 0; i < l.size; i++) { \
      name[i] = l.data[i]; \
    } \
    name[l.size] = '\0'; \
  } \

#define arr_size(x) (sizeof((x)) / sizeof(*(x)))

template<class T, class U>
T max(T a, U b) {
  return static_cast<T>((a < (T)b)? b: a);
}
template<class T, class U>
T min(T a, U b) {
  return static_cast<T>((a < (T)b)? a: b);
}

struct Timer {
  std::chrono::_V2::steady_clock::time_point start;
  std::chrono::_V2::steady_clock::time_point end;

  Timer()  { start = std::chrono::steady_clock::now(); }
  ~Timer() { 
    end = std::chrono::steady_clock::now(); 
  
    double delta = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
    print(delta);
  }
};

#define measure_scope() Timer ANONYMOUS_NAME;

template<class T, size_t N>
bool is_one_of(T c, const T (&x)[N]) {
  for_each(x) {
    if(c == *it) { return true; }
  }
  return false;
}

#endif
