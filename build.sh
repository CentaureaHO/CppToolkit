#!/bin/bash

if [[ " $@ " == *" help "* ]]; then
    echo "Available targets:"
    echo "  AnyTypeTest         - Build the AnyTypeTest target"
    echo "  ThreadPoolTest      - Build the ThreadPoolTest target"
    echo "  clean               - Clean the build directory"
    echo
    echo "Examples:"
    echo "  ./build.sh AnyTypeTest"
    echo "  ./build.sh -DUSE_RAW_POINTER=ON"
    echo "  ./build.sh -DTHREAD_POOL_SIZE=8 -DN=500000000 ThreadPoolTest"
    echo "  ./build.sh clean"
    exit 0
fi

cd ./cmake

if [ ! -f "Makefile" ]; then
    cmake .
fi

CMAKE_OPTIONS=""
TARGETS=()

THREAD_POOL_SIZE=4
N=1000000000

for arg in "$@"
do
    if [[ $arg == -D* ]]; then
        CMAKE_OPTIONS="$CMAKE_OPTIONS $arg"
    else
        TARGETS+=("$arg")
    fi
done

if [[ ! $CMAKE_OPTIONS =~ THREAD_POOL_SIZE ]]; then
    CMAKE_OPTIONS="$CMAKE_OPTIONS -DTHREAD_POOL_SIZE=$THREAD_POOL_SIZE"
fi

if [[ ! $CMAKE_OPTIONS =~ N ]]; then
    CMAKE_OPTIONS="$CMAKE_OPTIONS -DN=$N"
fi

if [ ${#CMAKE_OPTIONS} -gt 0 ]; then
    cmake . $CMAKE_OPTIONS
fi

if [ ${#TARGETS[@]} -gt 0 ]; then
    for target in "${TARGETS[@]}"; do
        cmake --build . --target "$target"
    done
else
    cmake --build .
fi

if [[ " ${TARGETS[*]} " == *" clean "* ]]; then
    cmake --build . --target clean
fi
