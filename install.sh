#!/bin/bash

cd build
if [ -n "$TEST_GAMMA" ]; then
  sudo cmake -DBUILD_TESTING=ON ..
  sudo make VERBOSE=1 -j4
  ./tests/global_tests

  read
else
  sudo cmake .. > /dev/null
  sudo make -j4 > /dev/null
fi

cd ../
sudo cp $(pwd)/gamma /usr/local/bin/
