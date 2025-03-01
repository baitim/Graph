cmake_minimum_required(VERSION 3.11)
project(unit)

set(INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)

find_package(GTest REQUIRED)

add_executable(unit_graph graph_unit_test.cpp)
target_sources(unit_graph
    PRIVATE
    FILE_SET HEADERS
    BASE_DIRS ${INCLUDE_DIR}
)
target_link_libraries(unit_graph PRIVATE GTest::GTest)

set(RUN_TESTS ./unit_graph --gtest_color=yes)
add_test(
    NAME unit_graph_target
    COMMAND ${RUN_TESTS}
)