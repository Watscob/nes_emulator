add_executable(
    nes_emulator
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
)

target_include_directories(
    nes_emulator
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(
    nes_emulator
    PRIVATE
    nes_emulator_lib
)
