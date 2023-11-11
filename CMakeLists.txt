if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

cmake_minimum_required(VERSION 3.7)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(SDL2_DIR "C:/Users/patri/Documents/Libs/SDL2-2.28.4/cmake/")

add_definitions(-std=c++17)

set(CXX_FLAGS "-Wall")
set(CMAKE_CXX_FLAGS, "${CXX_FLAGS}")
set(CMAKE_CXX_FLAGS_DEBUG "-g")

project(n_body_simulation)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS} src)

add_executable(n_body_simulation src/main.cpp src/controller.cpp src/renderer.cpp src/simulation.cpp src/particle_distribution.cpp)
#string(STRIP ${SDL2_LIBRARIES} SDL2_LIBRARIES)
target_link_libraries(n_body_simulation ${SDL2_LIBRARIES})
