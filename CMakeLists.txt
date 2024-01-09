cmake_minimum_required(VERSION 3.5.1)
project(cpp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Ofast")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

include_directories(include)

#jsoncpp
INCLUDE(FindPkgConfig)
find_package(Jsoncpp)
include_directories(${Jsoncpp_INCLUDE_DIR})

#gurobi
set(GUROBI_PATH ${CMAKE_SOURCE_DIR}/resource/gurobi912/linux64)
INCLUDE_DIRECTORIES(${GUROBI_PATH}/include/)
link_directories(${GUROBI_PATH}/lib)
set(GUROBI_FILE libgurobi_g++5.2.a libgurobi91.so)

set(THIRDPARTY_LIBS glog gflags ${Jsoncpp_LIBRARY} ${Z3_FILE} ${GUROBI_FILE})

add_subdirectory(basic)
add_subdirectory(autolifter)
add_subdirectory(exp)
add_subdirectory(baseline)
add_subdirectory(polygen)

add_executable(main main.cpp)
target_link_libraries(main exp_lib autolifter_lib baseline_lib polygen_lib basic_lib ${THIRDPARTY_LIBS})