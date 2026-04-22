find_package(GTest REQUIRED)
include(GoogleTest)

add_executable(
    test_nesemulator
    test_cpu.cpp
)

target_link_libraries(
    test_nesemulator
    PRIVATE
    nesemulator_lib
    GTest::GTest
    GTest::Main
)

gtest_discover_tests(test_nesemulator)
