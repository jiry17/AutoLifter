cmake_minimum_required(VERSION 3.5)

file(GLOB polygen_source "*.cpp")
add_library(polygen_lib STATIC ${polygen_source})