cmake_minimum_required(VERSION 2.6)
project (project)
 
set (PROJECT_INCLUDE_DIR ${PROJECT_SOURCE_DIR})
set (PROJECT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
 
set(PROJECT2_SRCS
    ${PROJECT_SOURCE_DIR}/main.cpp 
    )
 
include_directories("${PROJECT_BINARY_DIR}")
add_executable(execute ${PROJECT2_SRCS})
include_directories("${PROJECT_INCLUDE_DIR}")
