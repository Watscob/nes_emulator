add_executable(
    ${CMAKE_PROJECT_NAME}
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
)

target_include_directories(
    ${CMAKE_PROJECT_NAME}
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(
    ${CMAKE_PROJECT_NAME}
    PRIVATE
    ${CMAKE_PROJECT_NAME}_lib
)
