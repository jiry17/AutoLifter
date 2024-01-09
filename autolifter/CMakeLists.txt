cmake_minimum_required(VERSION 3.5)

file(GLOB autolifter_src "*.cpp")
add_library(autolifter_lib STATIC ${autolifter_src})