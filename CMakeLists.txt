cmake_minimum_required(VERSION 3.13)
project(opengl C)

find_package(SDL2 REQUIRED)
find_package(OpenGL REQUIRED)
include_directories(
        ${OPENGL_INCLUDE_DIR}
        ${SDL2_INCLUDE_DIRS}
)

set(CMAKE_C_STANDARD 99)

add_executable(opengl main.c)
target_link_libraries(opengl ${SDL2_LIBRARIES}
        ${OPENGL_LIBRARIES})