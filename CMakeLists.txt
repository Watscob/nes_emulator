cmake_minimum_required(VERSION 3.28)

project(
    nesemulator
    VERSION 1.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(
    -Wall
    -Wextra
    -Werror
    -O3
    -flto
)

add_subdirectory(src)
add_subdirectory(tests)
