   /*

   Program: conf (configurator) *This system consists of conf.c, conf_2.c,
   conf.h, conf_2.h, parser.y (a yacc source file) and lexan.l (a lex
   source file)

   Usage: conf < filenm

   Program Description: This is the configurator program (essentially a
   compiler) whose input is  a source file in CSL (Configuration
   Specification Language) and whose output is the information required by
   the DPC (Distributed Process Controller) to be able to start the
   application and individual processes in the correct sequence, and to be
   able to connect the data stores referred to in individual modules to
   their correct physical counterparts as specified in the CSL source.

   File: lexan.l

   Developers: Hasnain Rangwalla, Yuan Shi

   Last Change: 9/22/93 

   */

%{
#include "y.tab.h"
extern char integer[1024];
extern char quot_string[1024];
extern char string[1024];
extern int integer_ref;
extern int quot_string_ref;
extern int string_ref;
extern int yylval;
extern int yacc_debug;
int lex_debug;

#define TRUE 1
#define FALSE 0
#define  LEX_DEBUG if (lex_debug) printf
/* return integer value */
#define  RET_INTEGER strcpy(integer, yytext);
/* return found string value */
#define RET_QUOT_STRING strcpy(quot_string, &yytext[1]);
#define  RET_STRING strcpy(string, yytext);
%}
 
COMMENT		("/*"([^*]*)("*"[^/][^*]*)*"*/")
W		([ \t\n]+)
INTEGER		([0-9]+)
STRING		([a-zA-Z0-9+$%/._-]+)
QUOT_STRING	('.*')
MCOLON		([Mm]:)
FCOLON		([Ff]:)
SCOLON		([Ss]:)
ARROW		("->")
C_DEBUG         [Cc]_[Dd][Ee][Bb][Uu][Gg] 
R_DEBUG         [Rr]_[Dd][Ee][Bb][Uu][Gg]
R_LOG		[Rr]_[Ll][Oo][Gg] 
CONFIGURATION	[Cc][Oo][Nn][Ff][Ii][Gg][Uu][Rr][Aa][Tt][Ii][Oo][Nn]
STORAGE_LOC     [Ss][Tt][Oo][Rr][Aa][Gg][Ee]_[Ll][Oo][Cc]
EXEC_LOC        [Ee][Xx][Ee][Cc]_[Ll][Oo][Cc]
THRESHOLD	[Tt][Hh][Rr][Ee][Ss][Hh][Oo][Ll][Dd]
FACTOR		[Ff][Aa][Cc][Tt][Oo][Rr]
DEBUG		[Dd][Ee][Bb][Uu][Gg]
MODE		[Mm][Oo][Dd][Ee]
PARM		[Pp]([0-9]+)
GRP		[Gg][Rr][Pp]
ISAM		[Ii][Ss][Aa][Mm]
PIPE		[Pp][Ii][Pp][Ee]
DEV		[Dd][Ee][Vv]
MDL		[Mm][Dd][Ll]
NODE_ID         [Nn][Oo][Dd][Ee]_[Ii][Dd]
TYPE		[Tt][Yy][Pp][Ee]
OS              [Oo][Ss]
RECSZ           [Rr][Ee][Cc][Ss][Zz] 
SCREEN          [Ss][Cc][Rr][Ee][Ee][Nn]
SEQ		[Ss][Ee][Qq]
TERM            [Tt][Ee][Rr][Mm]
TS		[Tt][Ss]
STREAM		[Ss][Tt][Rr][Ee][Aa][Mm]
SLAVE		[Ss][Ll][Aa][Vv][Ee]
%%
{W}		;
{COMMENT}	{ LEX_DEBUG ("\n[COMMENT]");}

{CONFIGURATION}	{ LEX_DEBUG ("\n[CONFIGURATION]");
		  RET_STRING;
                  return(CONFIGURATION);
                }
{MODE}		{ LEX_DEBUG ("\n[MODE]");
		  RET_STRING;
		  return(MODE);
		}
{FACTOR}	{ LEX_DEBUG ("\n[FACTOR]");
		  RET_STRING;
		  return(FACTOR);
		}
{THRESHOLD}	{ LEX_DEBUG ("\n[THRESHOLD]");
		  RET_STRING;
		  return(THRESHOLD);
		}
{DEBUG}		{ LEX_DEBUG ("\n[DEBUG]");
		  RET_STRING;
		  return(DEBUG);
		}
{PARM}		{ LEX_DEBUG ("\n[PARM]");
		  RET_STRING;
		  return(PARM);
		}
{STORAGE_LOC}   { LEX_DEBUG ("\n[STORAGE_LOC]");
                  RET_STRING;
                  return(STORAGE_LOC);
                }
{EXEC_LOC}      { LEX_DEBUG("\n[EXEC_LOC]");
                  RET_STRING;
                  return(EXEC_LOC);
                }
{C_DEBUG}       { LEX_DEBUG("\n[C_DEBUG]");
                  RET_STRING ;
                  return(C_DEBUG);
                } 
{R_DEBUG}         { LEX_DEBUG("\n[R_DEBUG]");
                  RET_STRING ;
                  return(R_DEBUG);
                }               
{R_LOG}		{ LEX_DEBUG("\n[R_LOG]");
		  RET_STRING;
		  return(R_LOG); 
		}

{GRP}		{ LEX_DEBUG ("\n[GRP]");
		  RET_STRING;
                  return(GRP);
                }
{FCOLON}	{ LEX_DEBUG ("\n[FCOLON]");
		  RET_STRING;
                  return(FCOLON);
                }
{ISAM}		{ LEX_DEBUG ("\n[ISAM]");
		  RET_STRING;
                  return(ISAM);
                }
{PIPE}		{ LEX_DEBUG ("\n[PIPE]");
		  RET_STRING;
                  return(PIPE);
                }
{TS}		{ LEX_DEBUG ("\n[TS]");
		  RET_STRING;
		  return(TS);
		}
{DEV}		{ LEX_DEBUG ("\n[DEV]");
		  RET_STRING;
                  return(DEV);
                }
{MCOLON}	{ LEX_DEBUG ("\n[MCOLON]");
		  RET_STRING;
                  return(MCOLON);
                }
{MDL}		{ LEX_DEBUG ("\n[MDL]");
		  RET_STRING;
                  return(MDL);
                }
{NODE_ID}	{ LEX_DEBUG ("\n[NODE_ID]");
		  RET_STRING;
                  return(NODE_ID);
                }
{OS}		{ LEX_DEBUG ("\n[OS]");
		  RET_STRING;
                  return(OS);
                }
{RECSZ}		{ LEX_DEBUG ("\n[RECSZ]");
		  RET_STRING;
                  return(RECSZ);
                }
{SCOLON}	{ LEX_DEBUG ("\n[SCOLON]");
		  RET_STRING;
                  return(SCOLON);
                }
{SCREEN}	{ LEX_DEBUG ("\n[SCREEN]");
		  RET_STRING;
                  return(SCREEN);
                }
{SEQ}		{ LEX_DEBUG ("\n[SEQ]");
		  RET_STRING;
                  return(SEQ);
                }
{TYPE}		{ LEX_DEBUG ("\n[TYPE]");
		  RET_STRING;
                  return(TYPE);
                }
{TERM}		{ LEX_DEBUG ("\n[TERM]");
		  RET_STRING;
                  return(TERM);
                }
{ARROW}		{ LEX_DEBUG ("\n[ARROW]");
		  RET_STRING;
                  return(ARROW);
                }
{STREAM}	{ LEX_DEBUG ("\n[STREAM]");
		  RET_STRING;
		  return(STREAM);
		}
{SLAVE}		{ LEX_DEBUG ("\n[SLAVE]");
		  RET_STRING;
		  return(SLAVE);
		}
^"#"ld		{ lex_debug = TRUE; 
		  LEX_DEBUG ("\n[LEX_DBG]");
                }
^"#"yd		{ yacc_debug = TRUE; 
		  LEX_DEBUG ("\n[YACC_DBG]");
                }
{INTEGER}	{ LEX_DEBUG ("\n[INTEGER=%s]", yytext);
                  RET_INTEGER;
                  return(INTEGER);
                }
{STRING}	{ LEX_DEBUG ("\n[STRING=%s]", yytext);
                  RET_STRING;
                  return(STRING);
                }
{QUOT_STRING}	{ LEX_DEBUG ("\n[QUOT_STRING=%s]", yytext);
		  yytext[yyleng-1] = '\0';
                  RET_QUOT_STRING;
                  return(QUOT_STRING);
                }
.		{ LEX_DEBUG ("\n[OTHER_CHAR=%c]", yytext[0]);
		  yylval = yytext[0];
		  return(yytext[0]);
		}

