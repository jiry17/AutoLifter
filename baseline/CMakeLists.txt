cmake_minimum_required(VERSION 3.5)

file(GLOB relish_source "relish/*.cpp")
file(GLOB esolver_source "esolver/*.cpp")
file(GLOB oe_source "oe/*.cpp")
add_library(baseline_lib STATIC ${relish_source} ${esolver_source} ${oe_source})