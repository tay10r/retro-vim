cmake_minimum_required(VERSION 3.9.6)

find_package(Curses REQUIRED)

set(compat_sources
	misccmds.cpp)

add_executable(vim
	${compat_sources}
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
