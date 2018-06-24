cmake_minimum_required(VERSION 3.10)
project(buffer_cycle)

set(CMAKE_CXX_STANDARD 17)

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++17 -pedantic")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address,undefined -D_GLIBCXX_DEBUG")
endif()

add_executable(buffer_cycle main.cpp circular_buffer.h)

add_executable(test_deque
        gtest/gtest-all.cc
        gtest/gtest.h
        gtest/gtest_main.cc
        counted.cpp
        counted.h
        fault_injection.cpp
        fault_injection.h
        tests.cpp circular2.h)

target_link_libraries(test_deque -lgmp -lgmpxx -lpthread)
