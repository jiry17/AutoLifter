cmake_minimum_required(VERSION 3.5.1)
project(cpp)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

include_directories(. ./basic ./polygen ./solver ./exp ./hfta)
INCLUDE_DIRECTORIES(/home/jiry/2021A/gurobi912/linux64/include/)

ADD_DEFINITIONS(-DDEBUG)

file(GLOB basic_files "basic/*.h" "basic/*.cpp")
file(GLOB solver_files "solver/*.h" "solver/*.cpp")
file(GLOB exp_files "exp/*.h" "exp/*.cpp")
file(GLOB polygen_files "polygen/*.h" "polygen/*.cpp")
file(GLOB hfta_files "hfta/*.h" "hfta/*.cpp")
add_library(basic_lib ${basic_files})
add_library(solver_lib ${solver_files} ${hfta_files})
add_library(exp_lib ${exp_files})
add_library(polygen_lib ${polygen_files})

INCLUDE(FindPkgConfig)
find_package(Jsoncpp)
include_directories(${Jsoncpp_INCLUDE_DIR})
link_directories(/home/jiry/2021A/gurobi912/linux64/lib/)


add_executable(main main.cpp)
target_link_libraries(main exp_lib solver_lib polygen_lib basic_lib ${Jsoncpp_LIBRARY} glog gflags libgurobi_g++5.2.a libgurobi91.so)