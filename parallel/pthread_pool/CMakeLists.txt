cmake_minimum_required(VERSION 3.10)

project(pthread_pool)

set(CMAKE_CXX_STANDARD 20)

include_directories(include)

file(GLOB SOURCES src/*.cpp main.cpp)

add_executable(main ${SOURCES})