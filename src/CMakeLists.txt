add_library(
    nesemulator_lib
    STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/cpu.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/memory.cpp
)

target_include_directories(
    nesemulator_lib
    PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

add_executable(
    nesemulator
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
)

target_link_libraries(
    nesemulator
    PRIVATE
    nesemulator_lib
)
