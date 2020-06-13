#include <cassert>
#include <cstdlib>
#include <utility>
#include <iostream>

#include "array.h"

int main() {
  array<int> a;
  a.push_back(1);
  std::cout << a[0] << std::endl;
}
