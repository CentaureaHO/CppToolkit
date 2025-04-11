
mkdir -p build && cd build

cmake -DENABLE_DIFF_TEST=ON -DCMAKE_BUILD_TYPE=Release ..
make