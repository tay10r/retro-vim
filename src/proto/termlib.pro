/* termlib.c */
int tgetent __PARMS((char *tbuf, char *term));
int getent __PARMS((char *tbuf, char *term, struct __stdio *termcap));
int nextent __PARMS((char *tbuf, struct __stdio *termcap));
int tgetflag __PARMS((char *id));
int tgetnum __PARMS((char *id));
char *tgetstr __PARMS((char *id, char **buf));
char *tgoto __PARMS((char *cm, int col, int line));
int tputs __PARMS((char *cp, int affcnt, void (*outc)(unsigned int)));
