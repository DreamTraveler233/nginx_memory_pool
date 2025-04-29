cmake_minimum_required(VERSION 3.28)
project(nginx_memory_pool)

set(CMAKE_CXX_STANDARD 20)

add_executable(nginx_memory_pool ngx_mem_pool.cpp
        ngx_mem_pool.h
        test.cpp)
