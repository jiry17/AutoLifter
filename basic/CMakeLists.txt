cmake_minimum_required(VERSION 3.5)

file(GLOB basic_src "*.cpp")
add_library(basic_lib STATIC ${basic_src})