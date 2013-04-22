# include "stdio.h"
# define U(x) ((x)&0377)
# define NLSTATE yyprevious=YYNEWLINE
# define BEGIN yybgin = yysvec + 1 +
# define INITIAL 0
# define YYLERR yysvec
# define YYSTATE (yyestate-yysvec-1)
# define YYOPTIM 1
# define YYLMAX 200
# define output(c) putc(c,yyout)
# define input() (((yytchar=yysptr>yysbuf?U(*--yysptr):getc(yyin))==10?(yylineno++,yytchar):yytchar)==EOF?0:yytchar)
# define unput(c) {yytchar= (c);if(yytchar=='\n')yylineno--;*yysptr++=yytchar;}
# define yymore() (yymorfg=1)
# define ECHO fprintf(yyout, "%s",yytext)
# define REJECT { nstr = yyreject(); goto yyfussy;}
int yyleng; extern char yytext[];
int yymorfg;
extern char *yysptr, yysbuf[];
int yytchar;
FILE *yyin /*={stdin}*/, *yyout ={stdout};
extern int yylineno;
struct yysvf { 
	struct yywork *yystoff;
	struct yysvf *yyother;
	int *yystops;};
struct yysvf *yyestate;
extern struct yysvf yysvec[], *yybgin;
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
   Developers: Hasnain Rangwalla
   Last Change: 6/28/89
   */
# include "y.tab.h";
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
 
# define YYNEWLINE 10
yylex(){
int nstr; extern int yyprevious;
while((nstr = yylook()) >= 0)
yyfussy: switch(nstr){
case 0:
if(yywrap()) return(0); break;
case 1:
	;
break;
case 2:
{ LEX_DEBUG ("\n[COMMENT]");}
break;
case 3:
{ LEX_DEBUG ("\n[CONFIGURATION]");
		  RET_STRING;
                  return(CONFIGURATION);
                }
break;
case 4:
	{ LEX_DEBUG ("\n[MODE]");
		  RET_STRING;
		  return(MODE);
		}
break;
case 5:
{ LEX_DEBUG ("\n[FACTOR]");
		  RET_STRING;
		  return(FACTOR);
		}
break;
case 6:
{ LEX_DEBUG ("\n[THRESHOLD]");
		  RET_STRING;
		  return(THRESHOLD);
		}
break;
case 7:
	{ LEX_DEBUG ("\n[DEBUG]");
		  RET_STRING;
		  return(DEBUG);
		}
break;
case 8:
	{ LEX_DEBUG ("\n[PARM]");
		  RET_STRING;
		  return(PARM);
		}
break;
case 9:
  { LEX_DEBUG ("\n[STORAGE_LOC]");
                  RET_STRING;
                  return(STORAGE_LOC);
                }
break;
case 10:
     { LEX_DEBUG("\n[EXEC_LOC]");
                  RET_STRING;
                  return(EXEC_LOC);
                }
break;
case 11:
       { LEX_DEBUG("\n[C_DEBUG]");
                  RET_STRING ;
                  return(C_DEBUG);
                }
break;
case 12:
        { LEX_DEBUG("\n[R_DEBUG]");
                  RET_STRING ;
                  return(R_DEBUG);
                }
break;
case 13:
		{ LEX_DEBUG("\n[R_LOG]");
		  RET_STRING;
		  return(R_LOG); 
		}
break;
case 14:
	{ LEX_DEBUG ("\n[GRP]");
		  RET_STRING;
                  return(GRP);
                }
break;
case 15:
{ LEX_DEBUG ("\n[FCOLON]");
		  RET_STRING;
                  return(FCOLON);
                }
break;
case 16:
	{ LEX_DEBUG ("\n[ISAM]");
		  RET_STRING;
                  return(ISAM);
                }
break;
case 17:
	{ LEX_DEBUG ("\n[PIPE]");
		  RET_STRING;
                  return(PIPE);
                }
break;
case 18:
	{ LEX_DEBUG ("\n[TS]");
		  RET_STRING;
		  return(TS);
		}
break;
case 19:
	{ LEX_DEBUG ("\n[DEV]");
		  RET_STRING;
                  return(DEV);
                }
break;
case 20:
{ LEX_DEBUG ("\n[MCOLON]");
		  RET_STRING;
                  return(MCOLON);
                }
break;
case 21:
	{ LEX_DEBUG ("\n[MDL]");
		  RET_STRING;
                  return(MDL);
                }
break;
case 22:
{ LEX_DEBUG ("\n[NODE_ID]");
		  RET_STRING;
                  return(NODE_ID);
                }
break;
case 23:
	{ LEX_DEBUG ("\n[OS]");
		  RET_STRING;
                  return(OS);
                }
break;
case 24:
		{ LEX_DEBUG ("\n[RECSZ]");
		  RET_STRING;
                  return(RECSZ);
                }
break;
case 25:
{ LEX_DEBUG ("\n[SCOLON]");
		  RET_STRING;
                  return(SCOLON);
                }
break;
case 26:
{ LEX_DEBUG ("\n[SCREEN]");
		  RET_STRING;
                  return(SCREEN);
                }
break;
case 27:
	{ LEX_DEBUG ("\n[SEQ]");
		  RET_STRING;
                  return(SEQ);
                }
break;
case 28:
	{ LEX_DEBUG ("\n[TYPE]");
		  RET_STRING;
                  return(TYPE);
                }
break;
case 29:
	{ LEX_DEBUG ("\n[TERM]");
		  RET_STRING;
                  return(TERM);
                }
break;
case 30:
	{ LEX_DEBUG ("\n[ARROW]");
		  RET_STRING;
                  return(ARROW);
                }
break;
case 31:
{ LEX_DEBUG ("\n[STREAM]");
		  RET_STRING;
		  return(STREAM);
		}
break;
case 32:
	{ LEX_DEBUG ("\n[SLAVE]");
		  RET_STRING;
		  return(SLAVE);
		}
break;
case 33:
	{ lex_debug = TRUE; 
		  LEX_DEBUG ("\n[LEX_DBG]");
                }
break;
case 34:
	{ yacc_debug = TRUE; 
		  LEX_DEBUG ("\n[YACC_DBG]");
                }
break;
case 35:
{ LEX_DEBUG ("\n[INTEGER=%s]", yytext);
                  RET_INTEGER;
                  return(INTEGER);
                }
break;
case 36:
{ LEX_DEBUG ("\n[STRING=%s]", yytext);
                  RET_STRING;
                  return(STRING);
                }
break;
case 37:
{ LEX_DEBUG ("\n[QUOT_STRING=%s]", yytext);
		  yytext[yyleng-1] = '\0';
                  RET_QUOT_STRING;
                  return(QUOT_STRING);
                }
break;
case 38:
	{ LEX_DEBUG ("\n[OTHER_CHAR=%c]", yytext[0]);
		  yylval = yytext[0];
		  return(yytext[0]);
		}
break;
case -1:
break;
default:
fprintf(yyout,"bad switch yylook %d",nstr);
} return(0); }
/* end of yylex */
int yyvstop[] ={
0,

38,
0,

1,
38,
0,

1,
0,

36,
38,
0,

38,
0,

38,
0,

36,
38,
0,

35,
36,
38,
0,

36,
38,
0,

36,
38,
0,

36,
38,
0,

36,
38,
0,

36,
38,
0,

36,
38,
0,

36,
38,
0,

36,
38,
0,

36,
38,
0,

36,
38,
0,

36,
38,
0,

36,
38,
0,

36,
38,
0,

38,
0,

36,
0,

37,
0,

30,
0,

35,
36,
0,

36,
0,

36,
0,

36,
0,

36,
0,

15,
0,

36,
0,

36,
0,

36,
0,

20,
0,

36,
0,

36,
0,

36,
0,

23,
36,
0,

8,
36,
0,

36,
0,

36,
0,

36,
0,

25,
0,

36,
0,

36,
0,

36,
0,

36,
0,

36,
0,

36,
0,

18,
36,
0,

36,
0,

36,
0,

36,
0,

36,
0,

19,
36,
0,

36,
0,

36,
0,

14,
36,
0,

36,
0,

21,
36,
0,

36,
0,

36,
0,

36,
0,

36,
0,

36,
0,

36,
0,

36,
0,

27,
36,
0,

36,
0,

36,
0,

36,
0,

36,
0,

36,
0,

36,
0,

33,
0,

34,
0,

2,
0,

36,
0,

36,
0,

36,
0,

36,
0,

36,
0,

16,
36,
0,

4,
36,
0,

36,
0,

17,
36,
0,

36,
0,

36,
0,

36,
0,

36,
0,

36,
0,

36,
0,

36,
0,

29,
36,
0,

36,
0,

28,
36,
0,

36,
0,

36,
0,

7,
36,
0,

36,
0,

36,
0,

36,
0,

24,
36,
0,

36,
0,

13,
36,
0,

36,
0,

32,
36,
0,

36,
0,

36,
0,

36,
0,

36,
0,

36,
0,

36,
0,

5,
36,
0,

36,
0,

36,
0,

26,
36,
0,

36,
0,

31,
36,
0,

36,
0,

36,
0,

11,
36,
0,

36,
0,

22,
36,
0,

12,
36,
0,

36,
0,

36,
0,

36,
0,

10,
36,
0,

36,
0,

36,
0,

36,
0,

36,
0,

6,
36,
0,

36,
0,

36,
0,

36,
0,

9,
36,
0,

36,
0,

3,
36,
0,
0};
# define YYTYPE unsigned char
struct yywork { YYTYPE verify, advance; } yycrank[] ={
0,0,	0,0,	1,3,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	1,4,	1,5,	
4,5,	4,5,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	26,0,	0,0,	0,0,	
27,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	4,5,	
0,0,	1,6,	0,0,	0,0,	
1,7,	2,24,	0,0,	1,3,	
0,0,	2,7,	1,8,	0,0,	
1,9,	1,10,	26,27,	2,8,	
9,29,	27,27,	0,0,	0,0,	
85,59,	0,0,	0,0,	0,0,	
0,0,	85,85,	0,0,	0,0,	
0,0,	0,0,	1,6,	1,6,	
1,11,	1,12,	1,13,	1,14,	
1,15,	1,6,	1,16,	8,28,	
0,0,	1,6,	1,17,	1,18,	
1,19,	1,20,	1,6,	1,21,	
1,22,	1,23,	1,6,	1,6,	
12,33,	1,6,	1,6,	1,6,	
6,25,	6,25,	0,0,	0,0,	
0,0,	0,0,	0,0,	6,25,	
0,0,	0,0,	6,25,	6,25,	
6,25,	6,25,	6,25,	6,25,	
6,25,	6,25,	6,25,	6,25,	
6,25,	6,25,	57,83,	13,34,	
15,37,	16,38,	18,42,	58,84,	
12,33,	6,25,	6,25,	6,25,	
6,25,	6,25,	6,25,	6,25,	
6,25,	6,25,	6,25,	6,25,	
6,25,	6,25,	6,25,	6,25,	
6,25,	6,25,	6,25,	6,25,	
6,25,	6,25,	6,25,	6,25,	
6,25,	6,25,	6,25,	13,34,	
15,37,	16,38,	18,42,	6,25,	
19,43,	6,25,	6,25,	6,25,	
6,25,	6,25,	6,25,	6,25,	
6,25,	6,25,	6,25,	6,25,	
6,25,	6,25,	6,25,	6,25,	
6,25,	6,25,	6,25,	6,25,	
6,25,	6,25,	6,25,	6,25,	
6,25,	6,25,	6,25,	7,26,	
21,46,	31,60,	90,109,	11,31,	
19,43,	94,111,	32,61,	7,26,	
7,0,	10,30,	10,30,	10,30,	
10,30,	10,30,	10,30,	10,30,	
10,30,	10,30,	10,30,	11,32,	
14,35,	33,62,	34,64,	24,57,	
36,65,	37,66,	21,47,	14,36,	
38,67,	135,139,	40,68,	0,0,	
21,46,	31,60,	7,26,	11,31,	
24,58,	7,27,	32,61,	0,0,	
7,26,	33,63,	0,0,	0,0,	
0,0,	7,26,	7,26,	0,0,	
0,0,	0,0,	0,0,	17,39,	
0,0,	33,62,	34,64,	0,0,	
36,65,	37,66,	41,69,	14,36,	
38,67,	17,40,	40,68,	7,26,	
7,26,	7,26,	7,26,	7,26,	
7,26,	7,26,	7,26,	7,26,	
17,41,	33,63,	7,26,	7,26,	
7,26,	7,26,	7,26,	7,26,	
7,26,	7,26,	7,26,	7,26,	
7,26,	42,70,	7,26,	7,26,	
7,26,	0,0,	41,69,	0,0,	
45,71,	17,40,	20,44,	20,44,	
20,44,	20,44,	20,44,	20,44,	
20,44,	20,44,	20,44,	20,44,	
17,41,	22,48,	23,53,	46,72,	
49,75,	23,54,	47,73,	50,76,	
51,77,	53,80,	22,49,	54,81,	
22,50,	42,70,	47,74,	20,45,	
23,55,	29,29,	52,78,	22,51,	
45,71,	52,79,	23,56,	56,82,	
60,87,	29,29,	29,29,	22,52,	
61,88,	62,89,	64,90,	0,0,	
0,0,	0,0,	23,53,	46,72,	
49,75,	23,54,	47,73,	50,76,	
51,77,	53,80,	22,49,	54,81,	
22,50,	65,91,	47,74,	20,45,	
23,55,	0,0,	52,78,	22,51,	
29,29,	52,79,	23,56,	56,82,	
60,87,	0,0,	29,59,	22,52,	
61,88,	62,89,	64,90,	29,29,	
29,29,	30,30,	30,30,	30,30,	
30,30,	30,30,	30,30,	30,30,	
30,30,	30,30,	30,30,	0,0,	
0,0,	65,91,	0,0,	0,0,	
0,0,	29,29,	29,29,	29,29,	
29,29,	29,29,	29,29,	29,29,	
29,29,	29,29,	67,92,	69,93,	
29,29,	29,29,	29,29,	29,29,	
29,29,	29,29,	29,29,	29,29,	
29,29,	29,29,	29,29,	70,94,	
29,29,	29,29,	29,29,	44,44,	
44,44,	44,44,	44,44,	44,44,	
44,44,	44,44,	44,44,	44,44,	
44,44,	59,85,	71,95,	72,96,	
0,0,	73,97,	67,92,	69,93,	
74,98,	59,85,	59,85,	75,99,	
77,100,	78,101,	79,102,	0,0,	
0,0,	80,103,	81,104,	70,94,	
82,105,	87,106,	88,107,	89,108,	
91,110,	96,112,	97,113,	98,114,	
99,115,	100,116,	101,117,	102,118,	
104,119,	0,0,	71,95,	72,96,	
59,85,	73,97,	0,0,	0,0,	
74,98,	0,0,	59,85,	75,99,	
77,100,	78,101,	79,102,	59,86,	
59,85,	80,103,	81,104,	0,0,	
82,105,	87,106,	88,107,	89,108,	
91,110,	96,112,	97,113,	98,114,	
99,115,	100,116,	101,117,	102,118,	
104,119,	59,85,	59,85,	59,85,	
59,85,	59,85,	59,85,	59,85,	
59,85,	59,85,	106,120,	107,121,	
59,85,	59,85,	59,85,	59,85,	
59,85,	59,85,	59,85,	59,85,	
59,85,	59,85,	59,85,	109,122,	
59,85,	59,85,	59,85,	110,123,	
111,124,	113,125,	115,126,	117,127,	
118,128,	119,129,	120,130,	121,131,	
122,132,	124,133,	125,134,	127,135,	
129,136,	130,137,	106,120,	107,121,	
132,138,	136,140,	137,141,	139,142,	
140,143,	141,144,	142,145,	144,146,	
145,147,	146,148,	148,149,	109,122,	
0,0,	0,0,	0,0,	110,123,	
111,124,	113,125,	115,126,	117,127,	
118,128,	119,129,	120,130,	121,131,	
122,132,	124,133,	125,134,	127,135,	
129,136,	130,137,	0,0,	0,0,	
132,138,	136,140,	137,141,	139,142,	
140,143,	141,144,	142,145,	144,146,	
145,147,	146,148,	148,149,	0,0,	
0,0};
struct yysvf yysvec[] ={
0,	0,	0,
yycrank+-1,	0,		0,	
yycrank+-6,	yysvec+1,	0,	
yycrank+0,	0,		yyvstop+1,
yycrank+3,	0,		yyvstop+3,
yycrank+0,	yysvec+4,	yyvstop+6,
yycrank+56,	0,		yyvstop+8,
yycrank+-178,	0,		yyvstop+11,
yycrank+13,	0,		yyvstop+13,
yycrank+10,	yysvec+6,	yyvstop+15,
yycrank+141,	yysvec+6,	yyvstop+18,
yycrank+104,	yysvec+6,	yyvstop+22,
yycrank+19,	yysvec+6,	yyvstop+25,
yycrank+27,	yysvec+6,	yyvstop+28,
yycrank+142,	yysvec+6,	yyvstop+31,
yycrank+34,	yysvec+6,	yyvstop+34,
yycrank+34,	yysvec+6,	yyvstop+37,
yycrank+173,	yysvec+6,	yyvstop+40,
yycrank+39,	yysvec+6,	yyvstop+43,
yycrank+69,	yysvec+6,	yyvstop+46,
yycrank+226,	yysvec+6,	yyvstop+49,
yycrank+111,	yysvec+6,	yyvstop+52,
yycrank+227,	yysvec+6,	yyvstop+55,
yycrank+217,	yysvec+6,	yyvstop+58,
yycrank+95,	0,		yyvstop+61,
yycrank+0,	yysvec+6,	yyvstop+63,
yycrank+-11,	yysvec+7,	0,	
yycrank+-14,	yysvec+7,	yyvstop+65,
yycrank+0,	0,		yyvstop+67,
yycrank+-300,	0,		0,	
yycrank+301,	yysvec+6,	yyvstop+69,
yycrank+103,	yysvec+6,	yyvstop+72,
yycrank+118,	yysvec+6,	yyvstop+74,
yycrank+135,	yysvec+6,	yyvstop+76,
yycrank+133,	yysvec+6,	yyvstop+78,
yycrank+0,	0,		yyvstop+80,
yycrank+137,	yysvec+6,	yyvstop+82,
yycrank+125,	yysvec+6,	yyvstop+84,
yycrank+143,	yysvec+6,	yyvstop+86,
yycrank+0,	0,		yyvstop+88,
yycrank+134,	yysvec+6,	yyvstop+90,
yycrank+170,	yysvec+6,	yyvstop+92,
yycrank+197,	yysvec+6,	yyvstop+94,
yycrank+0,	yysvec+6,	yyvstop+96,
yycrank+343,	yysvec+6,	yyvstop+99,
yycrank+192,	yysvec+6,	yyvstop+102,
yycrank+220,	yysvec+6,	yyvstop+104,
yycrank+222,	yysvec+6,	yyvstop+106,
yycrank+0,	0,		yyvstop+108,
yycrank+206,	yysvec+6,	yyvstop+110,
yycrank+210,	yysvec+6,	yyvstop+112,
yycrank+227,	yysvec+6,	yyvstop+114,
yycrank+223,	yysvec+6,	yyvstop+116,
yycrank+211,	yysvec+6,	yyvstop+118,
yycrank+213,	yysvec+6,	yyvstop+120,
yycrank+0,	yysvec+6,	yyvstop+122,
yycrank+227,	yysvec+6,	yyvstop+125,
yycrank+14,	0,		0,	
yycrank+19,	0,		0,	
yycrank+-400,	0,		0,	
yycrank+238,	yysvec+6,	yyvstop+127,
yycrank+243,	yysvec+6,	yyvstop+129,
yycrank+228,	yysvec+6,	yyvstop+131,
yycrank+0,	yysvec+6,	yyvstop+133,
yycrank+247,	yysvec+6,	yyvstop+136,
yycrank+245,	yysvec+6,	yyvstop+138,
yycrank+0,	yysvec+6,	yyvstop+140,
yycrank+297,	yysvec+6,	yyvstop+143,
yycrank+0,	yysvec+6,	yyvstop+145,
yycrank+306,	yysvec+6,	yyvstop+148,
yycrank+318,	yysvec+6,	yyvstop+150,
yycrank+333,	yysvec+6,	yyvstop+152,
yycrank+320,	yysvec+6,	yyvstop+154,
yycrank+336,	yysvec+6,	yyvstop+156,
yycrank+329,	yysvec+6,	yyvstop+158,
yycrank+342,	yysvec+6,	yyvstop+160,
yycrank+0,	yysvec+6,	yyvstop+162,
yycrank+326,	yysvec+6,	yyvstop+165,
yycrank+331,	yysvec+6,	yyvstop+167,
yycrank+345,	yysvec+6,	yyvstop+169,
yycrank+340,	yysvec+6,	yyvstop+171,
yycrank+349,	yysvec+6,	yyvstop+173,
yycrank+351,	yysvec+6,	yyvstop+175,
yycrank+0,	0,		yyvstop+177,
yycrank+0,	0,		yyvstop+179,
yycrank+-14,	yysvec+59,	0,	
yycrank+0,	0,		yyvstop+181,
yycrank+348,	yysvec+6,	yyvstop+183,
yycrank+356,	yysvec+6,	yyvstop+185,
yycrank+352,	yysvec+6,	yyvstop+187,
yycrank+87,	yysvec+6,	yyvstop+189,
yycrank+345,	yysvec+6,	yyvstop+191,
yycrank+0,	yysvec+6,	yyvstop+193,
yycrank+0,	yysvec+6,	yyvstop+196,
yycrank+90,	yysvec+6,	yyvstop+199,
yycrank+0,	yysvec+6,	yyvstop+201,
yycrank+335,	yysvec+6,	yyvstop+204,
yycrank+360,	yysvec+6,	yyvstop+206,
yycrank+356,	yysvec+6,	yyvstop+208,
yycrank+359,	yysvec+6,	yyvstop+210,
yycrank+360,	yysvec+6,	yyvstop+212,
yycrank+365,	yysvec+6,	yyvstop+214,
yycrank+366,	yysvec+6,	yyvstop+216,
yycrank+0,	yysvec+6,	yyvstop+218,
yycrank+349,	yysvec+6,	yyvstop+221,
yycrank+0,	yysvec+6,	yyvstop+223,
yycrank+403,	yysvec+6,	yyvstop+226,
yycrank+390,	yysvec+6,	yyvstop+228,
yycrank+0,	yysvec+6,	yyvstop+230,
yycrank+411,	yysvec+6,	yyvstop+233,
yycrank+409,	yysvec+6,	yyvstop+235,
yycrank+419,	yysvec+6,	yyvstop+237,
yycrank+0,	yysvec+6,	yyvstop+239,
yycrank+408,	yysvec+6,	yyvstop+242,
yycrank+0,	yysvec+6,	yyvstop+244,
yycrank+416,	yysvec+6,	yyvstop+247,
yycrank+0,	yysvec+6,	yyvstop+249,
yycrank+424,	yysvec+6,	yyvstop+252,
yycrank+419,	yysvec+6,	yyvstop+254,
yycrank+425,	yysvec+6,	yyvstop+256,
yycrank+413,	yysvec+6,	yyvstop+258,
yycrank+428,	yysvec+6,	yyvstop+260,
yycrank+421,	yysvec+6,	yyvstop+262,
yycrank+0,	yysvec+6,	yyvstop+264,
yycrank+433,	yysvec+6,	yyvstop+267,
yycrank+431,	yysvec+6,	yyvstop+269,
yycrank+0,	yysvec+6,	yyvstop+271,
yycrank+434,	yysvec+6,	yyvstop+274,
yycrank+0,	yysvec+6,	yyvstop+276,
yycrank+425,	yysvec+6,	yyvstop+279,
yycrank+423,	yysvec+6,	yyvstop+281,
yycrank+0,	yysvec+6,	yyvstop+283,
yycrank+441,	yysvec+6,	yyvstop+286,
yycrank+0,	yysvec+6,	yyvstop+288,
yycrank+0,	yysvec+6,	yyvstop+291,
yycrank+114,	yysvec+6,	yyvstop+294,
yycrank+433,	yysvec+6,	yyvstop+296,
yycrank+445,	yysvec+6,	yyvstop+298,
yycrank+0,	yysvec+6,	yyvstop+300,
yycrank+435,	yysvec+6,	yyvstop+303,
yycrank+444,	yysvec+6,	yyvstop+305,
yycrank+429,	yysvec+6,	yyvstop+307,
yycrank+435,	yysvec+6,	yyvstop+309,
yycrank+0,	yysvec+6,	yyvstop+311,
yycrank+442,	yysvec+6,	yyvstop+314,
yycrank+449,	yysvec+6,	yyvstop+316,
yycrank+438,	yysvec+6,	yyvstop+318,
yycrank+0,	yysvec+6,	yyvstop+320,
yycrank+440,	yysvec+6,	yyvstop+323,
yycrank+0,	yysvec+6,	yyvstop+325,
0,	0,	0};
struct yywork *yytop = yycrank+550;
struct yysvf *yybgin = yysvec+1;
char yymatch[] ={
00  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,011 ,012 ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
011 ,01  ,01  ,01  ,'$' ,'$' ,01  ,01  ,
01  ,01  ,'*' ,'$' ,01  ,01  ,'$' ,'/' ,
'0' ,'0' ,'0' ,'0' ,'0' ,'0' ,'0' ,'0' ,
'0' ,'0' ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,'A' ,'B' ,'C' ,'D' ,'E' ,'F' ,'G' ,
'H' ,'I' ,'$' ,'$' ,'L' ,'M' ,'N' ,'O' ,
'P' ,'Q' ,'R' ,'S' ,'T' ,'U' ,'V' ,'$' ,
'X' ,'Y' ,'Z' ,01  ,01  ,01  ,01  ,'$' ,
01  ,'A' ,'B' ,'C' ,'D' ,'E' ,'F' ,'G' ,
'H' ,'I' ,'$' ,'$' ,'L' ,'M' ,'N' ,'O' ,
'P' ,'Q' ,'R' ,'S' ,'T' ,'U' ,'V' ,'$' ,
'X' ,'Y' ,'Z' ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
0};
char yyextra[] ={
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0};
/*	ncform	4.1	83/08/11	*/

int yylineno =1;
# define YYU(x) x
# define NLSTATE yyprevious=YYNEWLINE
char yytext[YYLMAX];
struct yysvf *yylstate [YYLMAX], **yylsp, **yyolsp;
char yysbuf[YYLMAX];
char *yysptr = yysbuf;
int *yyfnd;
extern struct yysvf *yyestate;
int yyprevious = YYNEWLINE;
yylook(){
	register struct yysvf *yystate, **lsp;
	register struct yywork *yyt;
	struct yysvf *yyz;
	int yych;
	struct yywork *yyr;
# ifdef LEXDEBUG
	int debug;
# endif
	char *yylastch;
	/* start off machines */
# ifdef LEXDEBUG
	debug = 0;
# endif
	if (!yymorfg)
		yylastch = yytext;
	else {
		yymorfg=0;
		yylastch = yytext+yyleng;
		}
	for(;;){
		lsp = yylstate;
		yyestate = yystate = yybgin;
		if (yyprevious==YYNEWLINE) yystate++;
		for (;;){
# ifdef LEXDEBUG
			if(debug)fprintf(yyout,"state %d\n",yystate-yysvec-1);
# endif
			yyt = yystate->yystoff;
			if(yyt == yycrank){		/* may not be any transitions */
				yyz = yystate->yyother;
				if(yyz == 0)break;
				if(yyz->yystoff == yycrank)break;
				}
			*yylastch++ = yych = input();
		tryagain:
# ifdef LEXDEBUG
			if(debug){
				fprintf(yyout,"unsigned char ");
				allprint(yych);
				putchar('\n');
				}
# endif
			yyr = yyt;
			if ( (int)yyt > (int)yycrank){
				yyt = yyr + yych;
				if (yyt <= yytop && yyt->verify+yysvec == yystate){
					if(yyt->advance+yysvec == YYLERR)	/* error transitions */
						{unput(*--yylastch);break;}
					*lsp++ = yystate = yyt->advance+yysvec;
					goto contin;
					}
				}
# ifdef YYOPTIM
			else if((int)yyt < (int)yycrank) {		/* r < yycrank */
				yyt = yyr = yycrank+(yycrank-yyt);
# ifdef LEXDEBUG
				if(debug)fprintf(yyout,"compressed state\n");
# endif
				yyt = yyt + yych;
				if(yyt <= yytop && yyt->verify+yysvec == yystate){
					if(yyt->advance+yysvec == YYLERR)	/* error transitions */
						{unput(*--yylastch);break;}
					*lsp++ = yystate = yyt->advance+yysvec;
					goto contin;
					}
				yyt = yyr + YYU(yymatch[yych]);
# ifdef LEXDEBUG
				if(debug){
					fprintf(yyout,"try fall back character ");
					allprint(YYU(yymatch[yych]));
					putchar('\n');
					}
# endif
				if(yyt <= yytop && yyt->verify+yysvec == yystate){
					if(yyt->advance+yysvec == YYLERR)	/* error transition */
						{unput(*--yylastch);break;}
					*lsp++ = yystate = yyt->advance+yysvec;
					goto contin;
					}
				}
			if ((yystate = yystate->yyother) && (yyt= yystate->yystoff) != yycrank){
# ifdef LEXDEBUG
				if(debug)fprintf(yyout,"fall back to state %d\n",yystate-yysvec-1);
# endif
				goto tryagain;
				}
# endif
			else
				{unput(*--yylastch);break;}
		contin:
# ifdef LEXDEBUG
			if(debug){
				fprintf(yyout,"state %d char ",yystate-yysvec-1);
				allprint(yych);
				putchar('\n');
				}
# endif
			;
			}
# ifdef LEXDEBUG
		if(debug){
			fprintf(yyout,"stopped at %d with ",*(lsp-1)-yysvec-1);
			allprint(yych);
			putchar('\n');
			}
# endif
		while (lsp-- > yylstate){
			*yylastch-- = 0;
			if (*lsp != 0 && (yyfnd= (*lsp)->yystops) && *yyfnd > 0){
				yyolsp = lsp;
				if(yyextra[*yyfnd]){		/* must backup */
					while(yyback((*lsp)->yystops,-*yyfnd) != 1 && lsp > yylstate){
						lsp--;
						unput(*yylastch--);
						}
					}
				yyprevious = YYU(*yylastch);
				yylsp = lsp;
				yyleng = yylastch-yytext+1;
				yytext[yyleng] = 0;
# ifdef LEXDEBUG
				if(debug){
					fprintf(yyout,"\nmatch ");
					sprint(yytext);
					fprintf(yyout," action %d\n",*yyfnd);
					}
# endif
				return(*yyfnd++);
				}
			unput(*yylastch);
			}
		if (yytext[0] == 0  /* && feof(yyin) */)
			{
			yysptr=yysbuf;
			return(0);
			}
		yyprevious = yytext[0] = input();
		if (yyprevious>0)
			output(yyprevious);
		yylastch=yytext;
# ifdef LEXDEBUG
		if(debug)putchar('\n');
# endif
		}
	}
yyback(p, m)
	int *p;
{
if (p==0) return(0);
while (*p)
	{
	if (*p++ == m)
		return(1);
	}
return(0);
}
	/* the following are only used in the lex library */
yyinput(){
	return(input());
	}
yyoutput(c)
  int c; {
	output(c);
	}
yyunput(c)
   int c; {
	unput(c);
	}
