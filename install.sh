#!/bin/bash -e


mkdir -p build
cd build
cmake -DBUILD_TESTING=ON -Wall ..
make VERBOSE=1
./test/global_tests
cd -


sudo cp $(pwd)/gamma /usr/local/bin/
