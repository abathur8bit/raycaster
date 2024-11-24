cmake_minimum_required(VERSION 3.13.4)
project(raycaster)

set(CMAKE_CXX_STANDARD 14)
set(_CRT_SECURE_NO_WARNINGS 1)

if(WIN32)
    include_directories(/workspace/sdl/SDL2/include)
    link_directories(/workspace/sdl/SDL2/lib/x86)
    link_directories(/workspace/sdl/SDL2/lib/x86)
    link_directories("/workspace/cxxtest-4.4")
    add_executable(raycaster main.cpp)
    target_link_libraries(raycaster PRIVATE SDL2main SDL2)
else()
    find_package(SDL2 REQUIRED COMPONENTS SDL2)
    add_executable(raycaster main.cpp)
    target_link_libraries(raycaster PRIVATE SDL2::SDL2)
endif()

