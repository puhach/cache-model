cmake_minimum_required(VERSION 3.10)

project(HitMiss)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(hitmiss main.cpp bitarray.h bitarray.cpp cacheline.h cacheset.h cpucache.h pipt.h lru.h nmru.h test.cpp) 

