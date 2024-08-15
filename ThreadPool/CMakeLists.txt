project(ThreadPool)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ThreadPool.cpp
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(ThreadPoolTest ${SOURCES})

option(THREAD_POOL_SIZE "Size of the thread pool" 4)
option(N "Number of ones to sum" 1000000000)

target_compile_definitions(ThreadPoolTest PRIVATE THREAD_POOL_SIZE=${THREAD_POOL_SIZE} N=${N})
