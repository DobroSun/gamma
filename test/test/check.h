#ifndef GAMMA_TEST_CHECK_H
#define GAMMA_TEST_CHECK_H

#include <iostream>

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

#define cyan(A) ANSI_COLOR_CYAN A ANSI_COLOR_RESET
#define blue(A) ANSI_COLOR_BLUE A ANSI_COLOR_RESET
#define yellow(A) ANSI_COLOR_YELLOW A ANSI_COLOR_RESET
#define green(A) ANSI_COLOR_GREEN A ANSI_COLOR_RESET
#define red(A) ANSI_COLOR_RED A ANSI_COLOR_RESET


template<class T, class C>
bool eq(T &&a, C &&b) {
  return a == b;
}
template<class T, class C>
bool neq(T &&a, C &&b) {
  return a != b;
}

#define EQ(A, B) \
  if(eq(A, B)) { \
    std::cout << "EQ " green("[passed]") " " << A << " is equal to " << B << std::endl; \
  } else { \
    std::cout << "EQ " red("[fail]") " expected " << A << " to be equal to " << B << std::endl; \
  }


#endif
