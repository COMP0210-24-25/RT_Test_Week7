cmake_minimum_required(VERSION 3.16)
project(RayTracingTesting
  VERSION 0.0.1
  LANGUAGES CXX
)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "-Wall -Wsign-conversion -g")

find_package(Catch2 3 REQUIRED)
enable_testing() 

include(ExternalProject)

ExternalProject_Add(
    RayTracerProject
    SOURCE_DIR ${CMAKE_SOURCE_DIR}/..
    BINARY_DIR ${CMAKE_BINARY_DIR}/raytrace_build
    INSTALL_COMMAND ""
)

add_executable(TestWeek7 ${SOURCE_FILES} testWeek7.cpp)
add_dependencies(TestWeek7 RayTracerProject)

target_link_libraries(TestWeek7 PUBLIC ${CMAKE_BINARY_DIR}/raytrace_build/lib/libRayTracing.so Catch2::Catch2WithMain)
target_include_directories(TestWeek7 PUBLIC ${CMAKE_SOURCE_DIR}/../include)