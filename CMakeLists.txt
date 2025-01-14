cmake_minimum_required(VERSION 3.27)
project(FPSOnline)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE Release)

set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR})
add_definitions(-DSRC_DIR="${SRC_DIR}")

add_subdirectory(engine)
#add_subdirectory(src/game)
add_subdirectory(src/lab)

add_executable(Tests 
    tests/test_main.cpp)
target_include_directories(Tests PRIVATE tests)
target_link_libraries(Tests PRIVATE Engine)
