cmake_minimum_required(VERSION 3.25)
project(joenhle_cpp_wheel)

set(CMAKE_CXX_STANDARD 14)

add_executable(joenhle_cpp_wheel
        lock_free_queue/lock_free_queue.hpp
        test/lock_free_queue_test.cpp)
target_link_libraries(joenhle_cpp_wheel pthread gtest)