cmake_minimum_required(VERSION 3.10.2)
project(snake)
 
set(CMAKE_CXX_STANDARD 11)
 
add_executable(snake main.cpp joystick.cpp)

find_package(PythonLibs REQUIRED)
include_directories(${PYTHON_INCLUDE_DIRS})
target_link_libraries(snake ${PYTHON_LIBRARIES})