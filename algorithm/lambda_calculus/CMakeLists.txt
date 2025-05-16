cmake_minimum_required(VERSION 3.10)

SET(CMAKE_C_COMPILER gcc)
SET(CMAKE_CXX_COMPILER g++)

project(lambda_calculus CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")

file(GLOB SRCS src/*.cpp)

add_executable(lc
    main.cpp
    ${SRCS}
)

add_executable(bf
    brainfuck.cpp
)

target_include_directories(lc PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(bf PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
