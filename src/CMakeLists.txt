cmake_minimum_required(VERSION 3.9.6)

find_package(Curses REQUIRED)
find_package(GTest REQUIRED)

set(compat_sources
	misccmds.cpp)

add_library(vimlib
	${compat_sources}
	environment.hpp
	environment.inl)

target_compile_options(vimlib PRIVATE -Wall -Wextra -Werror -Wfatal-errors)

target_compile_features(vimlib PUBLIC cxx_std_17)

add_executable(vim
	alloc.c
	buffers.c
	charset.c
	cmdline.c
	csearch.c
	digraph.c
	edit.c
	fileio.c
	help.c
	linefunc.c
	main.c
	mark.c
	message.c
	misccmds.c
	normal.c
	ops.c
	param.c
	quickfix.c
	regexp.c
	regsub.c
	screen.c
	script.c
	search.c
	storage.c
	tag.c
	term.c
	termlib.c
	undo.c
	unix.c
	version.c)

target_compile_definitions(vim PRIVATE BSD DIGRAPHS TERMCAP WILD_CARDS SOME_BUILTIN_TCAPS)

target_link_libraries(vim PRIVATE vimlib ${CURSES_LIBRARIES})

add_executable(vim_tests
	environment_tests.cpp)

target_link_libraries(vim_tests PRIVATE vimlib GTest::GTest GTest::Main)

add_test(NAME VimUnitTests COMMAND $<TARGET_FILE:vim_tests>)

set_target_properties(vim_tests
	PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}"
	OUTPUT_NAME run_tests)
