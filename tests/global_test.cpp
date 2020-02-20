#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <iostream>

#include "tests/mock_fact.hpp"

// Wanna check if MainWindow->run() returns true
// On closing with SDL_QUIT.


int main(int argc, char** argv) {
  MockFactory fact;
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
