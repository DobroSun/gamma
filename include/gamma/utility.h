#ifndef GAMMA_UTILITY_H
#define GAMMA_UTILITY_H

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



#define printi(A) do { printf("%i\n", (A)); } while(0)
#define printu(A) do { printf("%u\n", (A)); } while(0)
#define printl(A) do { printf("%lu\n", (A)); } while(0)
#define printd(A) do { printf("%f\n", (A)); } while(0)
#define printc(A) do { printf("%c\n", (A)); } while(0)
#define printb(A) do { if((A)) puts("true"); else puts("false"); } while(0)
#define printFPS                                 \
  auto begin = std::chrono::steady_clock::now(); \
  defer {                                        \
    auto end = std::chrono::steady_clock::now(); \
    std::cout << 1000000000 / std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count() << std::endl; \
  }

#define for_each(c_array) for(auto it = c_array; *it != '\0'; it++)
template<class T, class C>
bool in(const T *x, C val) {
  for_each(x) {
    if(*it == val) {
      return true;
    }
  }
  return false;
}
template<class T>
const T *slice(const T *x, unsigned index) {
  auto tmp = x;
  tmp += index;
  return tmp;
}

#define do_times(n, function) \
  { \
    for(decltype(n) i = 0; i < n; i++) { \
      function(); \
    } \
  } while(0)

#endif
