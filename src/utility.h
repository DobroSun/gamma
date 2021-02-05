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


#define for_each(c_array) for(auto it = c_array; *it != '\0'; it++)

struct literal {
  const char *data = NULL;
  size_t      size = 0;

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
  literal(const literal &)            = default;
  literal &operator=(const literal &) = default;
  literal(literal &&)                 = default;
  literal &operator=(literal &&)      = default;

  char operator[](size_t i) const {
    assert(i < size);
    return data[i];
  }
};
inline bool operator==(char s, literal l) { return l.size == 1 && l.data[0] == s; }
inline bool operator==(literal l, char s) { return l.size == 1 && l.data[0] == s; }

inline bool operator==(const char *s, literal l) { return l.size && !strncmp(s, l.data, l.size); }
inline bool operator==(literal l, const char *s) { return l.size && !strncmp(s, l.data, l.size); }
inline bool operator==(string s, literal l)      { return l.size == s.size && !strncmp(s.data, l.data, l.size); }
inline bool operator==(literal l, string s)      { return l.size == s.size && !strncmp(s.data, l.data, l.size); }
inline bool operator!=(const char *s, literal l) { return !(s == l); }
inline bool operator!=(literal l, const char *s) { return !(s == l); }
inline bool operator!=(string s, literal l)      { return !(s == l); }
inline bool operator!=(literal l, string s)      { return !(s == l); }

inline bool operator==(literal l1, literal l2) {
  if(l1.size == l2.size) {
    return !strncmp(l1.data, l2.data, l1.size);
  } else {
    return false;
  }
}

inline bool operator!=(literal l1, literal l2) { return !(l1 == l2); }


#define to_literal(a) literal(a.data, a.size)
#define to_string(a)  string(a.data, a.size)

inline std::ostream& operator<<(std::ostream &os, literal l) {
  for(size_t i = 0; i < l.size; i++) {
    os << l.data[i];
  }
  return os;
}

#define static_string_from_literal(name, l) \
  char name[l.size+1]; \
  memcpy(name, l.data, l.size); \
  name[l.size] = '\0';

inline char *dynamic_string_from_literal(literal l) {
  char *r = (char *)malloc(l.size+1);
  memcpy(r, l.data, l.size);
  r[l.size] = '\0';
  return r;
}


#define array_size(x) (sizeof((x)) / sizeof(*(x)))

template<class T, class U>
T max(T a, U b) { return static_cast<T>((a < static_cast<T>(b))? b: a); }

template<class T, class U>
T min(T a, U b) { return static_cast<T>((a < static_cast<T>(b))? a: b); }


struct Print {
  literal sep = ", ", end = "\n";

  void operator()() {
    std::cout << end;
    sep = ", ", end = "\n";
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
  
    double delta = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
    print.sep = "";
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


template<class T, class U, size_t N>
bool is_one_of(const T c, const U (&x)[N]) {
  for(size_t i = 0; i < N; i++) {
    if(c == x[i]) { return true; }
  }
  return false;
}

#endif
