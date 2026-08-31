find_package(SDL3 REQUIRED)

add_library(
    ${CMAKE_PROJECT_NAME}_lib
    STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nes_bus.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nes_cartridge.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nes_cpu.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nes_ppu_memory.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nes_ppu.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nes.cpp
)

target_include_directories(
    ${CMAKE_PROJECT_NAME}_lib
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${SDL3_INCLUDE_DIRS}
)

target_link_libraries(
    ${CMAKE_PROJECT_NAME}_lib
    PRIVATE
    ${SDL3_LIBRARIES}
)
