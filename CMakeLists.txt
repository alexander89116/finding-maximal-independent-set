cmake_minimum_required(VERSION 3.16)
project(finding_maximal_independent_set)

set(CMAKE_CXX_STANDARD 17)

find_package(Boost 1.71.0 REQUIRED COMPONENTS log)
#include_directories(${Boost_INCLUDE_DIRS})

add_executable(finding_maximal_independent_set main.cpp graph.hpp subgraph.hpp finding-maximal-independent-set.hpp)

target_link_libraries(finding_maximal_independent_set Boost::log)