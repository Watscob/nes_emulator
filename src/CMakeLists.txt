if (NOT ${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
    find_package(SDL3 REQUIRED)
endif()

add_executable(
    nesemulator
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
)

target_include_directories(
    nesemulator
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(
    nesemulator
    PRIVATE
    nesemulator_lib
)

if (${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
    target_compile_options(
        nesemulator
        PRIVATE
        "--use-port=sdl3"
        "-Wno-experimental"
    )

    target_link_options(
        nesemulator
        PRIVATE
        "-fwasm-exceptions"
        "-s WASM=1"
        "--use-port=sdl3"
        "-s EXPORTED_FUNCTIONS=['_main']"
        "-s EXPORTED_RUNTIME_METHODS=['ccall', 'cwrap']"
        "-s MODULARIZE=1"
        "-s ALLOW_MEMORY_GROWTH=1"
        "-s FULL_ES2=1"
        "-s EXPORT_ES6=1"
    )
else()
    target_link_libraries(
        nesemulator
        PRIVATE
        ${SDL3_LIBRARIES}
    )

    target_include_directories(
        nesemulator
        PRIVATE
        ${SDL3_INCLUDE_DIRS}
    )
endif()
