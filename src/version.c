/* vi:ts=4:sw=4:tw=77
 *
 *
 * VIM - Vi IMitation
 *
 * Code Contributions By:	Bram Moolenaar			mool@oce.nl
 *							Tim Thompson			twitch!tjt
 *							Tony Andrews			onecom!wldrdg!tony 
 *							G. R. (Fred) Walter		watmath!watcgl!grwalter 
 */

/*
 Started with Stevie version 3.6 (Fish disk 217) - GRWalter (Fred)

 VIM 1.0	- Changed so many things that I felt that a new name was required
			(I didn't like the name Stevie that much: I don't have an ST).
			- VIM stands for "Vi IMitation".
			- New storage structure, MULTI-LEVEL undo and redo,
			improved screen output, removed an awful number of bugs,
			removed fixed size buffers, added counts to a lot of commands,
			added new commands, added new options, added 'smart indent',
			added recording mode, added script files, moved help to a file,
			etc. etc. etc.
			- Compiles under Manx/Aztec C 5.0. You can use "rez" to make VIM
			resident.
			- Bram Moolenaar (Mool)

 VIM 1.09 - spaces can be used in tags file instead of tabs (compatible with
			Manx ctags).

 VIM 1.10 - Csh not required anymore for CTRL-D. Search options /e and /s added.
			Shell option implemented. BS in replace mode does not delete
			character. Backspace, wrapmargin and tags options added.
			Added support for Manx's QuickFix mode (just like "Z").
			The ENV: environment variables instead of the Old Manx environment
			variables are now used, because Vim was compiled with version 5.0d
			of the compiler. "mool" library not used anymore. Added index to
			help screens.

 VIM 1.11 - removed bug that caused :e of same file, but with name in upper case,
			to re-edit that file.

 VIM 1.12 - The second character of several commands (e.g. 'r', 't', 'm') not
			:mapped anymore (UNIX vi does it like this, don't know why); Some
			operators did not work when doing a 'l' on the last character in
			a line (e.g. 'yl'); Added :mapping when executing registers;
			Removed vi incompatibility from 't' and 'T' commands; :mapping! also
			works for command line editing; Changed a few details to have Vim
			run the macros for solving a maze and Towers of Hanoi! It now also
			runs the Turing machine macros!

 VIM 1.13 - Removed a bug for !! on empty line. "$" no longer puts cursor at
			the end of the line when combined with an operator. Added
			automatic creation of a script file for recovery after a crash.
			Added "-r" option. Solved bug for not detecting end of script file.
			".bak" is now appended, thus "main.c" and "main.h" will have
			separate backup files.

 VIM 1.14 - Removed a few minor bugs. Added "-n" option to skip autoscript.
			Made options more Vi compatible. Improved ^C handling. On serious
			errors typahead and scripts are discarded. 'U' is now correctly
			undone with 'u'. Fixed showmatch() handling of 'x' and '\x'.
			Solved window size dependency for scripts by adding ":winsize"
			commands to scripts. This version released on Fish disk 591.

 VIM 1.15 - No extra return in recording mode (MCHAR instead of MLINE buffer).
			plural() argument is now a long. Search patterns shared between
			:g, :s and /. After recovery a message is given. Overflow of mapbuf
			is detected. Line number possible with :read. Error message when
			characters follow a '$' in a search pattern. Cause for crash
			removed: ":s/pat/repl/g" allocated not enough memory. Option
			"directory" added. Option "expandtab" added. Solved showmode non-
			functioning. Solved bug with window resizing. Removed some *NULL
			references. CTRL-], * and # commands now skips non-identifier
			characters. Added tag list, CTRL-T, :pop and :tags commands.
			Added jump list, CTRL-O and CTRL-I commands. Added "shiftround"
			option. Applied AUX and Lattice mods from Juergen Weigert.
			Finally made linenr_t a long, files can be > 65000 lines!
			:win command could be written to script file halfway a command.
			Option shelltype added. With ^V no mapping anymore.
			Added :move, :copy, :t, :mark and :k. Improved Ex address parsing.
			Many delimiters possible with :s.

 VIM 1.16 - Solved bug with zero line number in Ex range. Added file-number to
			jump list. Solved bug when scrolling downwards. Made tagstack vi
			compatible. Cmdline editing: CTRL-U instead of '@'. Made Vim DICE
			compatible. Included regexp improvements from Olaf Seibert,
			mapstring() removed. Removed bug with CTRL-U in insert mode.
			Count allowed before ". Added uppercase (file) marks. Added
			:marks command. Added joinspaces option. Added :jumps command. Made
			jumplist compatible with tag list. Added count to :next and :Next.

 VIM 1.17 - Removed '"' for Ex register name. Repaired stupid bug in tag code.
			Now compiled with Aztec 5.2a. Changed Arpbase.h for use with 2.04
			includes. Added repdel option. Improved :set listing. Added count
			to 'u' and CTRL-R commands. "vi:" and "ex:" in modelines must now
			be preceded with a blank. Option "+command" for command line and
		    :edit command added.

 VIM 1.18 - Screen was not updated when all lines deleted. Readfile() now
			puts cursor on first new line. Catch strange disk label.
			Endless "undo line missing" loop removed. With empty file 'O' would
			cause this. Added window size reset in windexit(). Flush .vim file
			only when buffer has been changed. Added the nice things from
			Elvis 1.5: Added "equalprg" and "ruler" option. Added quoting.
			Added third meaning to 'backspace' option: backspacing over start
			of insert. Added "-c {command}" command line option. Made generating
			of prototypes automatically. Added insert mode command CTRL-O and
			arrow keys. CTRL-T/CTRL-D now always insert/delete indent. When
			starting an edit on specified lnum there was redraw of first lines.
			Added 'inputmode' option. Added CTRL-A and CTRL-S commands. '`' is
			now exclusive (as it should be). Added digraphs as an option.
			Changed implementation of parameters. Added :wnext command.
			Added ':@r' command. Changed handling of CTRL-V in command line.
			Block macros now work. Added keyword lookup command 'K'. Added
			CTRL-N and CTRL-P to command line editing. For DOS 2.0x the Flush
			function is used for the autoscript file; this should solve the
			lockup bug. Added wait_return to msg() for long messages.

 VIM 1.19 - Changes from Juergen Weigert:
			Terminal type no longer restricted to machine console. New
			option -T terminal. New set option "term". Builtin termcap 
			entries for "amiga", "ansi", "atari", "nansi", "pcterm". 
			Ported to MSDOS. New set option "textmode" ("tx") to control 
			CR-LF translation. CTRL-U and CTRL-D scroll half a screen full,
			rather than 12 lines. New set option "writebackup" ("wb") to 
			disable even the 'backup when writing' feature.
			Ported to SunOS. Full termcap support. Does resize on SIGWINCH.

			Made storage.c portable. Added reading of ".vimrc". Added
			'helpfile' option. With quoting the first char of an empty line
			is inverted. Optimized screen updating a bit. Startup code 
			looks for VIMINIT variable and .vimrc file first. Added option
			helpfile. Solved bug of inserting deletes: redefined ISSPECIAL.
			Changed inchar() to use key codes from termcap. Added parameters
			for terminal codes. Replaced aux device handling by amiga window
			handling. Added optional termcap code. Added 'V', format
			operator.

 VIM 1.20 - wait_return only ignores CR, LF and space. 'V' also works for
            single line. No redrawing while formatting text. Added CTRL-Z.
			Added usage of termcap "ks" and "ke". Fixed showmatch().
			Added timeout option. Added newfile argument to readfile().

 VIM 1.21 - Added block mode. Added 'o' command for quoting. Added :set inv.
			Added pos2ptr(). Added repeating and '$' to Quoting.

 VIM 1.22 - Fixed a bug in doput() with count > 1.
			Port to linux by Juergen Weigert included.
			More unix semantics in writeit(), forceit flag ignores errors while 
			preparing backup file. For UNIX, backup is now copied, not moved.
			When the current directory is not writable, vim now tries a backup
			in the directory given with the backupdir option. For UNIX, raw mode
			has now ICRNL turned off, that allowes ^V^M. Makefiles for BSD,
			SYSV, and linux unified in makefile.unix. For MSDOS
			mch_get_winsize() implemented. Reimplemented builtin termcaps in
			term.c and term.h. set_term() now handles all cases. Even builtins
			when TERMCAP is defined. Show "..." while doing filename completion.

 VIM 1.23 -	Improved MSDOS version: Added function and cursor keys to builtin 
			pcterm. Replaced setmode by settmode, delay by vim_delay and 
			delline by dellines to avoid name conflicts. Made F1 help key.
			Renamed makecmdtab to mkcmdtab and cmdsearch to csearch for 
			8 char name limit. Wildcard expansion adds *.* to names without a 
			dot. Added shell execution.
			For unix: writeit() overwrites readonly files when forced write,
			more safety checks. Termcap buffer for linux now 2048 bytes.
			Expandone() no longer appends "*" to file name. Added "graphic"
			option. Added ':' command to quoting.
			
 VIM 1.24	Adjusted number of spaces inserted by dojoin(). MSDOS version uses 
			searchpath() to find helpfile. Fixed a few small problems. Fixed 
			nasty bug in getperm() for SAS 6.0. Removed second argument from 
			wait_return(). Script files accessed in binary mode with MSDOS. 
			Added 'u' and 'U' commands to quoting (make upper or lower case). 
			Added "CTRL-V [0-9]*" to enter any byte value. Fixed doput().
			Dodis() displays register 0. Added CTRL-B to insert mode. Attempt 
			to fix the lockup bug by adding Delay() to startscript(). -v 
			option now implies -n option. doformat() added to improve 'V' 
			command. Replace bool_t with int. Fixed handling of \& and ~ in
			regsub(). Added interrupt handling in msdos.c for ctrl-break and
			critical errors. Added scrolljump option. Added :stop. Added -d
			argument. Fixed bug in quickfix startup from cli. Fixed enforcer
			hit with aux:. Added CTRL-C handling to unix.c. Fixed "O<BS><CR>" 
			bug with autoindent. Worked around :cq not working by adding a 
			printf()!? Added default mapping for MSDOS PageUp etc. Fixed 
			cursor position after 'Y'. Added shift-cursor commands. Changed 
			ExpandFile() to keep names with errors. Added CLEAR and CURSUPD 
			arguments to updateScreen(). Fixed CTRL-@ after a change command.
			modname() changes '.' into '_'. Added emptyrows to screen.c. 
			Fixed redo of search with offset. Added count to 'z' command. 
			Made :so! work with :global. Added writing of cursor postition to 
			startscript(). Minimized terminal requirements. Fixed problem 
			with line in tags file with mixed spaces and tabs. Fixed problem 
			with pattern "\\" in :s and :g.
*/

char		   *Version = "VIM v1.24";
#if !defined(linux) && defined(BSD) || defined(SYSV)
char		   *longVersion = "Vi IMitation v1.24 (5 Jan 1993) by Bram Moolenaar";
#else
char		   *longVersion = "Vi IMitation v1.24 (" __DATE__ " " __TIME__ ") by Bram Moolenaar";
#endif
