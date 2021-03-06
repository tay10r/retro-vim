/* vi:ts=4:sw=4
 *
 * VIM - Vi IMitation
 *
 * Code Contributions By:	Bram Moolenaar			mool@oce.nl
 *							Tim Thompson			twitch!tjt
 *							Tony Andrews			onecom!wldrdg!tony 
 *							G. R. (Fred) Walter		watmath!watcgl!grwalter 
 */

#define EXTERN
#include "vim.h"
#include "globals.h"
#include "proto.h"
#include "param.h"

static void usage __PARMS((int));

	static void
usage(n)
	int n;
{
	register int i;
	static char *(use[]) = {"[file ..]\n",
							"-t tag\n",
							"+[command] file ..\n",
							"-c {command} file ..\n",
							"-e\n"};
	static char *(errors[]) =  {"Unknown option\n",			/* 0 */
								"Too many arguments\n",		/* 1 */
								"Argument missing\n",		/* 2 */
								};

	fprintf(stderr, errors[n]);
	fprintf(stderr, "usage:");
	for (i = 0; ; ++i)
	{
		fprintf(stderr, " vim [options] ");
		fprintf(stderr, use[i]);
		if (i == (sizeof(use) / sizeof(char *)) - 1)
			break;
		fprintf(stderr, "   or:");
	}
#ifdef AMIGA
	fprintf(stderr, "\noptions: -v -n -r -d device -s scriptin -w scriptout -T terminal\n");
#else
	fprintf(stderr, "\noptions: -v -n -r -s scriptin -w scriptout -T terminal\n");
#endif
	mch_windexit(1);
}

	void
main(argc, argv)
	int				argc;
	char		  **argv;
{
	char		   *initstr;		/* init string from the environment */
	char		   *term = NULL;	/* specified terminal name */
	char		   *fname = NULL;	/* file name from command line */
	char		   *command = NULL;	/* command from + option */
	int 			c;
	int				doqf = 0;

#ifdef DEBUG
# ifdef MSDOS
	OPENDEBUG("#debug#");
# else
	OPENDEBUG("/tmp/debug/vim");
# endif
#endif

/*
 * Check if we have an interactive window.
 * If not, open one with a newcli command (needed for :! to work).
 * check_win will also handle the -d argument (for the Amiga).
 */
	check_win(argc, argv);

	++argv;
	/*
	 * Process the command line arguments
	 * 		'-s scriptin'
	 *		'-w scriptout'
	 *		'-v'
	 *		'-n'
	 *		'-r'
	 *		'-T terminal'
	 */
	while (argc > 1 && argv[0][0] == '-' &&
			strchr("swvnrTd", c = argv[0][1]) != NULL && c)
	{
		--argc;
		switch (c)
		{
		case 'v':
			readonlymode = TRUE;
			p_ro = TRUE;
			/*FALLTHROUGH*/

		case 'n':
			p_uc = 0;
			break;

		case 'r':
			recoverymode = 1;
			break;
		
		default:	/* options with argument */
			++argv;
			--argc;
			if (argc < 1)
				usage(2);

			switch (c)
			{
			case 's':
				if ((scriptin[0] = fopen(argv[0],
#ifdef MSDOS
													"rb"
#else
													"r"
#endif
														)) == NULL)
				{
						fprintf(stderr, "cannot open %s for reading\n", argv[0]);
						mch_windexit(2);
				}
				break;
			
			case 'w':
				if ((scriptout = fopen(argv[0],
#ifdef MSDOS
													"ab"
#else
													"a"
#endif
														)) == NULL)
				{
						fprintf(stderr, "cannot open %s for output\n", argv[0]);
						mch_windexit(2);
				}
				break;

/*
 * The -T term option is always available and when TERMCAP is supported it
 * overrides the environment variable TERM.
 */
			case 'T':
				term = *argv;
				break;
			
		/*	case 'd':		This is ignored as it is handled in check_win() */
			}
		}
		++argv;
	}

	/*
	 * Allocate space for the generic buffer
	 */
	if ((IObuff = alloc(IOSIZE)) == NULL)
		mch_windexit(0);

	/* note that we may use mch_windexit() before mch_windinit()! */
	mch_windinit();
	set_init();			/* after mch_windinit because Rows is used */

	/*
	 * Process the other command line arguments.
	 */
	if (argc > 1)
	{
		c = argv[0][1];
		switch (argv[0][0])
		{
		  case '-':
		    switch (c)
			{
		  	case 'e':			/* -e QuickFix mode */
				if (argc != 2)
					usage(1);
				doqf = 1;
				break;

			case 'c':			/* -c {command} file .. */
				if (argc <= 3)
					usage(2);
				++argv;
				--argc;
				command = &(argv[0][0]);
				goto getfiles;

			case 't':			/* -t tag */
				if (argc < 3)
					usage(2);
				if (argc > 3)
					usage(1);
				++argv;
				stuffReadbuff(":ta ");
				stuffReadbuff(argv[0]);
				stuffReadbuff("\n");
				break;

			default:
				usage(0);
			}
			break;

		  case '+': 			/* + or +{number} or +/{pat} or +{command} */
			if (argc < 3)		/* no filename */
					usage(2);
			if (c == NUL)
				command = "$";
			else
				command = &(argv[0][1]);

getfiles:
			++argv;
			--argc;
			/*FALLTHROUGH*/

		  default:				/* must be a file name */
#if defined(WILD_CARDS) && !defined(UNIX)
			ExpandWildCards(argc - 1, argv, &numfiles, &files, TRUE, TRUE);
			if (numfiles != 0)
				fname = files[0];

#else
			files = argv;
			numfiles = argc - 1;
			fname = argv[0];
#endif
			if (numfiles > 1)
				printf("%d files to edit\n", numfiles);
			break;
		}
	}

	if (numfiles == 0)
		numfiles = 1;

	RedrawingDisabled = TRUE;
	filealloc();				/* Initialize storage structure */
	init_yank();				/* init yank buffers */
	termcapinit(term);			/* get terminal capabilities */

#ifdef MSDOS /* default mapping for some often used keys */
	domap(0, "#1 :help\r", 0);				/* F1 is help key */
	domap(0, "\236R i", 0);					/* INSERT is 'i' */
	domap(0, "\236S x", 0);					/* DELETE is 'x' */
	domap(0, "\236G 0", 0);					/* HOME is '0' */
	domap(0, "\236w H", 0);					/* CTRL-HOME is 'H' */
	domap(0, "\236O $", 0);					/* END is '$' */
	domap(0, "\236u L", 0);					/* CTRL-END is 'L' */
	domap(0, "\236I \002", 0);				/* PageUp is '^B' */
	domap(0, "\236\204 1G", 0);				/* CTRL-PageUp is '1G' */
	domap(0, "\236Q \006", 0);				/* PageDown is '^F' */
	domap(0, "\236v G", 0);					/* CTRL-PageDown is 'G' */
			/* insert mode */
	domap(0, "\236S \177", INSERT);			/* DELETE is <DEL> */
	domap(0, "\236G \017H", INSERT);		/* HOME is '^OH' */
	domap(0, "\236w \017H", INSERT);		/* CTRL-HOME is '^OH' */
	domap(0, "\236O \017L", INSERT);		/* END is '^OL' */
	domap(0, "\236u \017L", INSERT);		/* CTRL-END is '^OL' */
	domap(0, "\236I \017\002", INSERT);		/* PageUp is '^O^B' */
	domap(0, "\236\204 \017\061G", INSERT);	/* CTRL-PageUp is '^O1G' */
	domap(0, "\236Q \017\006", INSERT);		/* PageDown is '^O^F' */
	domap(0, "\236v \017G", INSERT);		/* CTRL-PageDown is '^OG' */
#endif

/*
 * Read the VIMINIT or EXINIT environment variable
 *		(commands are to be separated with '|').
 * If there is none, read initialization commands from "s:.vimrc" or "s:.exrc".
 */
	if ((initstr = getenv("VIMINIT")) != NULL || (initstr = getenv("EXINIT")) != NULL)
		docmdline((u_char *)initstr);
	else if (dosource(SYSVIMRC_FILE))
		dosource(SYSEXRC_FILE);

/*
 * read initialization commands from ".vimrc" or ".exrc" in current directory
 */
	if (dosource(VIMRC_FILE))
		dosource(EXRC_FILE);

/*
 * Call settmode here, so the T_KS may be defined by termcapinit and
 * redifined in .exrc.
 */
	settmode(1);

#ifdef AMIGA
	fname_case(fname);		/* set correct case for file name */
#endif
	setfname(fname);
	maketitle();
	if (Filename != NULL)
		readfile(Filename, (linenr_t)0, TRUE);
	else
		msg("Empty Buffer");

	setpcmark();
	startscript();				/* start writing to auto script file */

	if (recoverymode && !scriptin[curscript])	/* first do script file, then recover */
		openrecover();

	/* position the display and the cursor at the top of the file. */
	Topline = 1;
	Curpos.lnum = 1;
	Curpos.col = 0;
	Cursrow = Curscol = 0;

	if (doqf)
	{
		if (qf_init(p_ef))
			mch_windexit(3);
		qf_jump(0);
	}

	if (command)
		docmdline((u_char *)command);

	RedrawingDisabled = FALSE;
	updateScreen(NOT_VALID);

		/* start in insert mode (already taken care of for :ta command) */
	if (p_im && stuff_empty())
		stuffReadbuff("i");
/*
 * main command loop
 */
	for (;;)
	{
		adjustCurpos();
		cursupdate();	/* Figure out where the cursor is based on Curpos. */

		if (Quote.lnum)
			updateScreen(INVERTED);		/* update inverted part */
		setcursor();

		normal();						/* get and execute a command */
	}
	/*NOTREACHED*/
}

	void
getout(r)
	int 			r;
{
	windgoto((int)Rows - 1, 0);
	outchar('\r');
	outchar('\n');
	mch_windexit(r);
}
