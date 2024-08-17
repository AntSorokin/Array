cmake_minimum_required(VERSION 3.14)
project(Dynamic_Array VERSION 1.0.0)

add_library(Array INTERFACE)
add_library(Data_Structure::Array ALIAS Array)
target_include_directories(Array INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)

