cmake_minimum_required(VERSION 3.5.0)
project("cp1craft" C CXX)

message(STATUS "source dir: "  ${CMAKE_SOURCE_DIR})
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

find_package(Fbthrift REQUIRED)
find_package(Folly REQUIRED)
find_package(Gflags REQUIRED)
find_package(Glog REQUIRED)

find_package(Googletest REQUIRED)
