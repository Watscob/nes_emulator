cmake_minimum_required(VERSION 3.28)

project(
    nes_emulator
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

add_subdirectory(lib)
add_subdirectory(src)
