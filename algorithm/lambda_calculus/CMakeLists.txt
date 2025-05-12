cmake_minimum_required(VERSION 3.10)

SET(CMAKE_C_COMPILER gcc)
SET(CMAKE_CXX_COMPILER g++)

project(lambda_calculus CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

file(GLOB SRCS src/*.cpp)

add_executable(lc
    main.cpp
    ${SRCS}
)

target_include_directories(lc PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
