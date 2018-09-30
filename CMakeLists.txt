cmake_minimum_required(VERSION 3.7)
project(MCAS)

# MCAS is a CAS
# It assumes an already existing parsing mechanism that can convert to a tree.

set(CMAKE_CXX_STANDARD 17)

if (CMAKE_BUILD_TYPE EQUAL "DEBUG")
	add_definitions(-g)
endif()

add_definitions(-Wall)

add_library(cas src/cas/expr.cpp src/cas/op.cpp src/cas/simplify.cpp
	        src/cas/polynomial.cpp)
target_include_directories(cas INTERFACE src/cas)

add_executable(cas_test src/frontend/main.cpp src/frontend/postfix.cpp)
target_link_libraries(cas_test cas)
