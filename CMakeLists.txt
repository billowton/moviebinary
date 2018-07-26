#cmake_minimum_required(VERSION 3.9)
project(moviebinary)

set(CMAKE_CXX_STANDARD 98)

add_executable(moviebinary main.cpp)

set(SOURCE_FILES main.cpp)
find_package(OpenCV REQUIRED)
target_link_libraries(moviebinary ${OpenCV_LIBS})