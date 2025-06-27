cmake_minimum_required(VERSION 3.10)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(EXEC_NAME game)
project(${EXEC_NAME} LANGUAGES CXX)

include(FetchContent)

FetchContent_Declare(
    raylib
    GIT_REPOSITORY "https://github.com/raysan5/raylib.git"
    GIT_TAG "5.5"
)

set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_GAMES    OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(raylib)

add_executable(${EXEC_NAME}
    src/main.cpp
)
target_include_directories(${EXEC_NAME} PRIVATE src)
target_compile_features(${EXEC_NAME} PRIVATE cxx_std_20)
target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Og -ggdb3)
target_link_libraries(${PROJECT_NAME} PRIVATE raylib)
