cmake_minimum_required(VERSION 3.30)
project(end_to_end)

set(PYTHON_SCRIPT_RUN "${CMAKE_SOURCE_DIR}/tests/end_to_end/run_tests.py")
add_test(
    NAME run_graph_target
    COMMAND bash -c "python3 ${PYTHON_SCRIPT_RUN}"
)