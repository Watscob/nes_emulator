add_library(
    nesemulator_lib
    STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/bus.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/cartridge.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/cpu.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nes.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ppu.cpp
)

target_include_directories(
    nesemulator_lib
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)
