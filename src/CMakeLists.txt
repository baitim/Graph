cmake_minimum_required(VERSION 3.11)
project(src)

set(INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)

add_executable(graph graph.cpp)
target_sources(graph
    PRIVATE
    FILE_SET HEADERS
    BASE_DIRS ${INCLUDE_DIR}
)

add_library(Graph graph.cpp)
target_include_directories(Graph PUBLIC
    $<BUILD_INTERFACE:${INCLUDE_DIR}>
    $<INSTALL_INTERFACE:include>
)

install(TARGETS Graph EXPORT GraphTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)

install(DIRECTORY ${INCLUDE_DIR}/Graph DESTINATION include)

install(EXPORT GraphTargets
    FILE GraphConfig.cmake
    DESTINATION lib/cmake/Graph
)