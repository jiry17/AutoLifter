cmake_minimum_required(VERSION 3.5)

file(GLOB paradigm_source "paradigms/*.cpp")
file(GLOB dataset_source "dataset/*.cpp")
add_library(exp_lib STATIC ${paradigm_source} ${dataset_source})