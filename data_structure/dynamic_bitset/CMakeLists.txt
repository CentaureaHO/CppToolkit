cmake_minimum_required(VERSION 3.10)
project(dynamic_bitset)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -O2")
    # set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ffast-math -ftree-vectorize")
    
    if(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64|amd64|AMD64")
        include(CheckCXXCompilerFlag)
        CHECK_CXX_COMPILER_FLAG("-mpopcnt" COMPILER_SUPPORTS_POPCNT)
        if(COMPILER_SUPPORTS_POPCNT)
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mpopcnt")
        endif()
        
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")
    endif()
endif()

option(ENABLE_DIFF_TEST "Enable comparison tests with boost::dynamic_bitset" ON)

if(ENABLE_DIFF_TEST)
    find_package(Boost REQUIRED)
    if(Boost_FOUND)
        include_directories(${Boost_INCLUDE_DIRS})
        add_definitions(-DDIFF_TEST)
        message(STATUS "Boost found. DIFF_TEST enabled.")
    else()
        message(WARNING "Boost not found. DIFF_TEST disabled.")
    endif()
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(COMMON_SOURCE 
    src/dynamic_bitset.cpp
)

add_executable(test_functionality 
    ${COMMON_SOURCE}
    test_functionality.cpp
)

add_executable(test_performance 
    ${COMMON_SOURCE}
    test_performance.cpp
)

enable_testing()
add_test(NAME functionality_test COMMAND test_functionality)
