cmake_minimum_required(VERSION 3.28)

project(
    nesemulator
    VERSION 1.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(
    nesemulator
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
)

target_include_directories(
    nesemulator
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_compile_options(
    nesemulator
    PRIVATE
    -Wall
    -Wextra
    -Werror
)
