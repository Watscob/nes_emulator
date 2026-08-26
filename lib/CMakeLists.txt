add_library(
    nes_emulator_lib
    STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nes_bus.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nes_cpu.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nes.cpp
)

target_include_directories(
    nes_emulator_lib
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)
