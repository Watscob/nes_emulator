if (NOT ${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
    find_package(SDL3 REQUIRED)
endif()

add_executable(
    nesemulator
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/sdl.cpp
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
        "-s WASM=1"
        "--bind"
        "--use-port=sdl3"
        "-s MODULARIZE=1"
        "-s ALLOW_MEMORY_GROWTH=1"
        "-s FULL_ES2=1"
        "-s EXPORT_ES6=1"
    )

    file(GLOB JS_FILES "${CMAKE_CURRENT_BINARY_DIR}/*.js")
    file(GLOB WASM_FILES "${CMAKE_CURRENT_BINARY_DIR}/*.wasm")

    install(
        FILES ${JS_FILES} ${WASM_FILES}
        DESTINATION .
        PERMISSIONS OWNER_READ OWNER_WRITE
    )

    add_custom_command(
        TARGET nesemulator
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} --install . --prefix ${CMAKE_SOURCE_DIR}/web
        COMMENT "Install .js and .wasm files in ${CMAKE_INSTALL_PREFIX}"
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
