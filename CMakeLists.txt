cmake_minimum_required(VERSION 3.15)
project(joenhle_cpp_wheel)

set(CMAKE_CXX_STANDARD 14)

add_executable(joenhle_cpp_wheel
        lock_free_queue/lock_free_queue.hpp
        test/lock_free_queue_test.cpp
        thread_pool/thread.hpp
        thread_pool/task.hpp
        thread_pool/thread_pool.hpp
        test/thread_pool_test.cpp
        ipc/ipc_broker.hpp
        ipc/shm_broker.hpp
        test/ipc_shm_test/client_test.cpp
        test/ipc_shm_test/server_test.cpp
        transport_client/udp_broker.hpp
)
target_link_libraries(joenhle_cpp_wheel pthread gtest)
add_executable(client_broker_test test/udp/client_broker_test.cpp
        test/udp/client_broker_test.cpp)
add_executable(server_broker_test test/udp/server_broker_test.cpp
        test/udp/server_broker_test.cpp)