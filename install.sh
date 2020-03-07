#!/bin/bash -e


sudo mkdir -p build
cd build
sudo cmake -DBUILD_TESTING=ON -Wall ..
sudo make VERBOSE=1
./test/global_tests
cd -


sudo cp $(pwd)/gamma /usr/local/bin/
