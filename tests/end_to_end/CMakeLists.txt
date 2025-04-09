set(PYTHON_SCRIPT_RUN "${CMAKE_SOURCE_DIR}/tests/end_to_end/run_core.py")
add_test(
    NAME run_graph_core_target
    COMMAND bash -c "python3 ${PYTHON_SCRIPT_RUN}"
)

set(PYTHON_SCRIPT_RUN "${CMAKE_SOURCE_DIR}/tests/end_to_end/run_tests.py")
add_test(
    NAME run_graph_test_target
    COMMAND bash -c "python3 ${PYTHON_SCRIPT_RUN}"
)