project(AnyType)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/test.cpp
)

add_executable(AnyTypeTest ${SOURCES})

set_target_properties(AnyTypeTest PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    RUNTIME_OUTPUT_NAME "AnyTypeTest"
)

option(USE_RAW_POINTER "Use raw pointers instead of smart pointers" OFF)
if (USE_RAW_POINTER)
    target_compile_definitions(AnyTypeTest PRIVATE USE_RAW_POINTER)
endif ()