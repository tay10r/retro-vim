cmake_minimum_required(VERSION 3.9.6)

find_package(GTest REQUIRED)

add_executable(vim_unit_tests
	environment_tests.cpp)

target_link_libraries(vim_unit_tests PRIVATE vimlib GTest::GTest GTest::Main)

target_compile_options(vim_unit_tests PRIVATE -Wall -Wextra -Werror -Wfatal-errors)

add_test(NAME VimUnitTests COMMAND $<TARGET_FILE:vim_tests>)

set_target_properties(vim_unit_tests
	PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}"
	OUTPUT_NAME run_tests)
