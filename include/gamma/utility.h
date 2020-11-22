#ifndef GAMMA_UTILITY_H
#define GAMMA_UTILITY_H
#include <iostream> // for print, and printFPS.
#include <chrono>
#include <cstring> // strncmp.
#include <unistd.h>

inline char *concat(const char *a, const char *b) {
  char *ret;
  auto s1 = strlen(a);
  auto s2 = strlen(b);

  ret = (char *)malloc(sizeof(char) * (s1 + s2 + 1));
  assert(ret);

  memcpy(ret, a, sizeof(char) * s1);
  memcpy(ret + s1, b, sizeof(char) * s2);

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

#define do_times(n, function) \
  { \
    for(decltype(n) i = 0; i < n; i++) { \
      function(); \
    } \
  } while(0)

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

#define to_literal(a) literal{a.data(), a.size()}
inline bool operator==(const literal &l, const string_t &s) {
  return to_literal(s) == l;
}
inline bool operator==(const string_t &s, const literal &l) {
  return to_literal(s) == l;
}

inline std::ostream& operator<<(std::ostream &os, const literal &l) {
  for(auto i = 0u; i < l.size; i++) {
    os << l.data[i];
  }
  return os;
}

inline string_t to_string(const literal &l) {
  string_t r;
  for(size_t i = 0; i < l.size; i++) {
    r.push_back(l.data[i]);
  }
  return r;
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
#define MIN(a, b)   (((decltype((b)))(a) < (b))? (a): (b))
#define MAX(a, b)   (((decltype((a)))(b) < (a))? (a): (b))

#endif
