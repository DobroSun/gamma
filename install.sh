#!/bin/bash -e


# TODO:
# Somehow need to fix no cmake rebuilds.
mkdir -p build
cd build
cmake -DBUILD_TESTING=ON ..
make VERBOSE=1
./test/global_tests
cd -


sudo cp $(pwd)/gamma /usr/local/bin/
