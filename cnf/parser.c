/*    February '13, updated by Justin Y. Shi  */
#ifndef lint
static const char yysccsid[] = "@(#)yaccpar	1.9 (Berkeley) 02/21/93";
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define YYBYACC 1
#define YYMAJOR 1
#define YYMINOR 9
#define YYPATCH 20070509

#define YYEMPTY (-1)
#define yyclearin    (yychar = YYEMPTY)
#define yyerrok      (yyerrflag = 0)
#define YYRECOVERING (yyerrflag != 0)

extern int yyparse(void);

static int yygrowstack(void);
#define YYPREFIX "yy"
#line 26 "parser.y"
extern char application_name[];
extern char integer[1024];
extern char quot_string[1024];
extern char string[1024];

#define YACC_DEBUG(_msg_) if (yacc_debug) printf("\nyyparse: _msg_")
#define YACC_DEBUG2(_m1_, _m2_) if (yacc_debug) printf("\nyyparse: _m1_", _m2_)

int yacc_debug;
#line 33 "y.tab.c"
#define INTEGER 257
#define QUOT_STRING 258
#define STRING 259
#define CONFIGURATION 260
#define C_DEBUG 261
#define R_DEBUG 262
#define R_LOG 263
#define STORAGE_LOC 264
#define EXEC_LOC 265
#define FACTOR 266
#define THRESHOLD 267
#define MODE 268
#define DEBUG 269
#define MAX_MSGS 270
#define PARM 271
#define NODE_ID 272
#define OS 273
#define UNIX 274
#define MSDOS 275
#define VMS 276
#define MACOS 277
#define SCOLON 278
#define MCOLON 279
#define FCOLON 280
#define ARROW 281
#define TYPE 282
#define TS 283
#define SEQ 284
#define PIPE 285
#define POST 286
#define DEV 287
#define GRP 288
#define MDL 289
#define ISAM 290
#define TERM 291
#define SCREEN 292
#define STREAM 293
#define SLAVE 294
#define RECSZ 295
#define YYERRCODE 256
short yylhs[] = {                                        -1,
    0,    1,    2,    3,    3,    5,    5,    5,    6,    6,
    7,    7,    8,    4,    4,    9,    9,   13,   14,   16,
   10,   15,   15,   17,   12,   12,   11,   11,   19,   19,
   21,   21,   22,   22,   24,   18,   25,   20,   27,   23,
   23,   26,   26,   31,   28,   30,   30,   29,   33,   33,
   34,   35,   34,   36,   34,   34,   34,   34,   34,   34,
   34,   34,   37,   37,   37,   37,   37,   37,   32,
};
short yylen[] = {                                         2,
    2,    4,    2,    2,    0,    1,    1,    1,    0,    4,
    0,    4,    4,    0,    2,    1,    1,    0,    0,    0,
    9,    3,    0,    0,    4,    1,    4,    2,    2,    2,
    3,    0,    3,    0,    0,    3,    0,    3,    0,    4,
    1,    2,    1,    0,    3,    2,    0,    3,    2,    0,
    0,    0,    7,    0,    7,    3,    3,    3,    3,    3,
    3,    3,    1,    1,    1,    1,    1,    1,    1,
};
short yydefred[] = {                                      0,
    0,    0,    0,    0,    0,    0,    0,    1,   14,    0,
    6,    7,    8,    0,    0,    0,    0,    0,    4,    2,
    0,    0,    0,   18,   37,   35,   15,   16,   17,    0,
    0,    0,   10,   12,   13,    0,    0,    0,    0,   30,
   28,    0,   29,    0,   19,   44,   38,    0,    0,   36,
    0,    0,    0,    0,    0,    0,    0,    0,   42,    0,
   31,   27,    0,    0,    0,    0,   45,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,   25,
   20,   69,   46,   40,    0,    0,    0,    0,    0,    0,
    0,    0,   48,   49,   33,   23,   52,   54,   60,   59,
   61,   62,   58,   57,   63,   64,   65,   66,   67,   68,
   56,    0,    0,    0,    0,   21,    0,    0,   22,    0,
    0,   53,   55,
};
short yydgoto[] = {                                       2,
    3,    8,    9,   18,   10,   11,   12,   13,   27,   28,
   29,   45,   36,   55,  112,   96,   54,   53,   31,   32,
   40,   61,   47,   38,   37,   48,   57,   49,   59,   67,
   56,   83,   77,   78,  113,  114,  111,
};
short yysindex[] = {                                   -232,
  -32,    0, -230, -225,  -25,  -13,  -11,    0,    0, -230,
    0,    0,    0,   -8, -205, -203, -201, -233,    0,    0,
   -2,   -1,    2,    0,    0,    0,    0,    0,    0, -222,
    3, -221,    0,    0,    0, -196, -195, -195, -238,    0,
    0, -215,    0,    0,    0,    0,    0,    0,   26,    0,
 -214,    9, -212,   24,   27,   11,   29, -255,    0, -238,
    0,    0, -204, -185, -196, -183,    0, -195,   16,   17,
   18,   19,   20,   21,   22,   23,   44, -255, -221,    0,
    0,    0,    0,    0, -173, -172, -169, -168, -167, -166,
 -234, -245,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,  -29,   32,   35, -196,    0,   36,   37,    0, -163,
 -162,    0,    0,
};
short yyrindex[] = {                                      0,
    0,    0,    1,    0,    0,    0,    0,    0,    0,    1,
    0,    0,    0,    0,    0,    0,    0,   98,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,   40,
    0,   40,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,  -24,    0,    0,    0,  -42,  -41,    0,
   40,    0,   -1,    0,    0,  -40,    0,   59,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,   59,   40,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,
};
short yygindex[] = {                                      0,
    0,    0,   91,    0,    0,    0,    0,    0,    0,    0,
    0,  -60,    0,    0,    0,    0,    0,  -10,   63,  -39,
  -26,    0,  -31,    0,    0,    0,    0,    0,    0,    0,
    0,    0,   25,    0,    0,    0,    0,
};
#define YYTABLESIZE 281
short yytable[] = {                                      47,
    5,   39,   43,   47,   81,   43,   50,   30,   69,   70,
   71,   72,   73,   74,  115,   75,   41,   43,   47,   26,
   79,   24,  103,   79,  104,    4,   76,    1,   51,  116,
    5,    6,    7,   14,   26,   15,   84,  105,  106,  107,
   25,   26,  108,  109,   24,   25,   26,   16,  110,   17,
   20,   21,   95,   22,  119,   23,   33,   34,   39,   42,
   35,   41,   44,   46,   26,   58,   60,   62,   63,   64,
   65,   66,   68,   80,   25,   82,   85,   86,   87,   88,
   89,   90,   91,   92,   93,   97,   98,   99,  100,  117,
  102,  101,  118,  120,  121,  122,  123,    3,   32,   50,
   19,   52,   94,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,   41,   43,
   47,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    5,    5,
    5,
};
short yycheck[] = {                                      40,
    0,   44,   44,   44,   65,   32,   38,   18,  264,  265,
  266,  267,  268,  269,   44,  271,   59,   59,   59,   44,
   60,   46,  257,   63,  259,   58,  282,  260,   39,   59,
  261,  262,  263,  259,   59,   61,   68,  283,  284,  285,
  279,  280,  288,  289,  278,  279,  280,   61,  294,   61,
   59,  257,   79,  257,  115,  257,   59,   59,  281,  281,
   59,   59,  259,  259,  280,   40,  281,   59,  281,   46,
   44,   61,   44,  259,  279,  259,   61,   61,   61,   61,
   61,   61,   61,   61,   41,  259,  259,  257,  257,   58,
  257,  259,   58,   58,   58,  259,  259,    0,   59,   41,
   10,   39,   78,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,  281,  281,
  281,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,  278,  279,
  280,
};
#define YYFINAL 2
#ifndef YYDEBUG
#define YYDEBUG 0
#endif
#define YYMAXTOKEN 295
#if YYDEBUG
char *yyname[] = {
"end-of-file",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,"'('","')'",0,"'+'","','",0,"'.'",0,0,0,0,0,0,0,0,0,0,0,"':'","';'",
0,"'='",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"'['",0,"']'",
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,"INTEGER","QUOT_STRING","STRING","CONFIGURATION","C_DEBUG","R_DEBUG",
"R_LOG","STORAGE_LOC","EXEC_LOC","FACTOR","THRESHOLD","MODE","DEBUG","MAX_MSGS",
"PARM","NODE_ID","OS","UNIX","MSDOS","VMS","MACOS","SCOLON","MCOLON","FCOLON",
"ARROW","TYPE","TS","SEQ","PIPE","POST","DEV","GRP","MDL","ISAM","TERM",
"SCREEN","STREAM","SLAVE","RECSZ",
};
char *yyrule[] = {
"$accept : conf_prog",
"conf_prog : header csl_body",
"header : CONFIGURATION ':' STRING ';'",
"csl_body : debug_switch dec_repeat",
"debug_switch : debug debug_switch",
"debug_switch :",
"debug : comp_time_debug",
"debug : run_time_debug",
"debug : run_time_log",
"comp_time_debug :",
"comp_time_debug : C_DEBUG '=' INTEGER ';'",
"run_time_debug :",
"run_time_debug : R_DEBUG '=' INTEGER ';'",
"run_time_log : R_LOG '=' INTEGER ';'",
"dec_repeat :",
"dec_repeat : dec_repeat declarations",
"declarations : synonym",
"declarations : declaration",
"$$1 :",
"$$2 :",
"$$3 :",
"synonym : SCOLON $$1 s_name $$2 ',' s_name $$3 s_opt ';'",
"s_opt : s_opt ',' s_name",
"s_opt :",
"$$4 :",
"s_name : STRING $$4 '.' STRING",
"s_name : STRING",
"declaration : f_dec ARROW m_f_dec ';'",
"declaration : m_f_dec ';'",
"m_f_dec : m_dec f_dec_opt",
"m_f_dec : f_dec f_dec_opt",
"f_dec_opt : ARROW f_dec m_dec_opt",
"f_dec_opt :",
"m_dec_opt : ARROW m_dec f_dec_opt",
"m_dec_opt :",
"$$5 :",
"f_dec : FCOLON $$5 mf_names",
"$$6 :",
"m_dec : MCOLON $$6 mf_names",
"$$7 :",
"mf_names : mf_name $$7 ',' mf_names",
"mf_names : mf_name",
"mf_name : part1 part2",
"mf_name : part1",
"$$8 :",
"part1 : STRING $$8 p_name_opt",
"p_name_opt : '=' p_name",
"p_name_opt :",
"part2 : '(' attributes ')'",
"attributes : attribute attributes",
"attributes :",
"attribute :",
"$$9 :",
"attribute : STORAGE_LOC '=' STRING $$9 ':' ':' STRING",
"$$10 :",
"attribute : EXEC_LOC '=' STRING $$10 ':' ':' STRING",
"attribute : TYPE '=' org",
"attribute : PARM '=' STRING",
"attribute : PARM '=' INTEGER",
"attribute : THRESHOLD '=' INTEGER",
"attribute : FACTOR '=' INTEGER",
"attribute : MODE '=' STRING",
"attribute : DEBUG '=' INTEGER",
"org : TS",
"org : SEQ",
"org : PIPE",
"org : GRP",
"org : MDL",
"org : SLAVE",
"p_name : STRING",
};
#endif
#ifndef YYSTYPE
typedef int YYSTYPE;
#endif
#if YYDEBUG
#include <stdio.h>
#endif

/* define the initial stack-sizes */
#ifdef YYSTACKSIZE
#undef YYMAXDEPTH
#define YYMAXDEPTH  YYSTACKSIZE
#else
#ifdef YYMAXDEPTH
#define YYSTACKSIZE YYMAXDEPTH
#else
#define YYSTACKSIZE 500
#define YYMAXDEPTH  500
#endif
#endif

#define YYINITSTACKSIZE 500

int      yydebug;
int      yynerrs;
int      yyerrflag;
int      yychar;
short   *yyssp;
YYSTYPE *yyvsp;
YYSTYPE  yyval;
YYSTYPE  yylval;

/* variables for the parser stack */
static short   *yyss;
static short   *yysslim;
static YYSTYPE *yyvs;
static int      yystacksize;
/* allocate initial stack or double stack size, up to YYMAXDEPTH */
static int yygrowstack(void)
{
    int newsize, i;
    short *newss;
    YYSTYPE *newvs;

    if ((newsize = yystacksize) == 0)
        newsize = YYINITSTACKSIZE;
    else if (newsize >= YYMAXDEPTH)
        return -1;
    else if ((newsize *= 2) > YYMAXDEPTH)
        newsize = YYMAXDEPTH;

    i = yyssp - yyss;
    newss = (yyss != 0)
          ? (short *)realloc(yyss, newsize * sizeof(*newss))
          : (short *)malloc(newsize * sizeof(*newss));
    if (newss == 0)
        return -1;

    yyss  = newss;
    yyssp = newss + i;
    newvs = (yyvs != 0)
          ? (YYSTYPE *)realloc(yyvs, newsize * sizeof(*newvs))
          : (YYSTYPE *)malloc(newsize * sizeof(*newvs));
    if (newvs == 0)
        return -1;

    yyvs = newvs;
    yyvsp = newvs + i;
    yystacksize = newsize;
    yysslim = yyss + newsize - 1;
    return 0;
}

#define YYABORT goto yyabort
#define YYREJECT goto yyabort
#define YYACCEPT goto yyaccept
#define YYERROR goto yyerrlab
int
yyparse(void)
{
    register int yym, yyn, yystate;
#if YYDEBUG
    register const char *yys;

    if ((yys = getenv("YYDEBUG")) != 0)
    {
        yyn = *yys;
        if (yyn >= '0' && yyn <= '9')
            yydebug = yyn - '0';
    }
#endif

    yynerrs = 0;
    yyerrflag = 0;
    yychar = YYEMPTY;

    if (yyss == NULL && yygrowstack()) goto yyoverflow;
    yyssp = yyss;
    yyvsp = yyvs;
    *yyssp = yystate = 0;

yyloop:
    if ((yyn = yydefred[yystate]) != 0) goto yyreduce;
    if (yychar < 0)
    {
        if ((yychar = yylex()) < 0) yychar = 0;
#if YYDEBUG
        if (yydebug)
        {
            yys = 0;
            if (yychar <= YYMAXTOKEN) yys = yyname[yychar];
            if (!yys) yys = "illegal-symbol";
            printf("%sdebug: state %d, reading %d (%s)\n",
                    YYPREFIX, yystate, yychar, yys);
        }
#endif
    }
    if ((yyn = yysindex[yystate]) && (yyn += yychar) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == yychar)
    {
#if YYDEBUG
        if (yydebug)
            printf("%sdebug: state %d, shifting to state %d\n",
                    YYPREFIX, yystate, yytable[yyn]);
#endif
        if (yyssp >= yysslim && yygrowstack())
        {
            goto yyoverflow;
        }
        *++yyssp = yystate = yytable[yyn];
        *++yyvsp = yylval;
        yychar = YYEMPTY;
        if (yyerrflag > 0)  --yyerrflag;
        goto yyloop;
    }
    if ((yyn = yyrindex[yystate]) && (yyn += yychar) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == yychar)
    {
        yyn = yytable[yyn];
        goto yyreduce;
    }
    if (yyerrflag) goto yyinrecovery;

    yyerror("syntax error");

#ifdef lint
    goto yyerrlab;
#endif

yyerrlab:
    ++yynerrs;

yyinrecovery:
    if (yyerrflag < 3)
    {
        yyerrflag = 3;
        for (;;)
        {
            if ((yyn = yysindex[*yyssp]) && (yyn += YYERRCODE) >= 0 &&
                    yyn <= YYTABLESIZE && yycheck[yyn] == YYERRCODE)
            {
#if YYDEBUG
                if (yydebug)
                    printf("%sdebug: state %d, error recovery shifting\
 to state %d\n", YYPREFIX, *yyssp, yytable[yyn]);
#endif
                if (yyssp >= yysslim && yygrowstack())
                {
                    goto yyoverflow;
                }
                *++yyssp = yystate = yytable[yyn];
                *++yyvsp = yylval;
                goto yyloop;
            }
            else
            {
#if YYDEBUG
                if (yydebug)
                    printf("%sdebug: error recovery discarding state %d\n",
                            YYPREFIX, *yyssp);
#endif
                if (yyssp <= yyss) goto yyabort;
                --yyssp;
                --yyvsp;
            }
        }
    }
    else
    {
        if (yychar == 0) goto yyabort;
#if YYDEBUG
        if (yydebug)
        {
            yys = 0;
            if (yychar <= YYMAXTOKEN) yys = yyname[yychar];
            if (!yys) yys = "illegal-symbol";
            printf("%sdebug: state %d, error recovery discards token %d (%s)\n",
                    YYPREFIX, yystate, yychar, yys);
        }
#endif
        yychar = YYEMPTY;
        goto yyloop;
    }

yyreduce:
#if YYDEBUG
    if (yydebug)
        printf("%sdebug: state %d, reducing by rule %d (%s)\n",
                YYPREFIX, yystate, yyn, yyrule[yyn]);
#endif
    yym = yylen[yyn];
    if (yym)
        yyval = yyvsp[1-yym];
    else
        memset(&yyval, 0, sizeof yyval);
    switch (yyn)
    {
case 1:
#line 105 "parser.y"
{
		  YACC_DEBUG2(configuration %s parsed, application_name);
		}
break;
case 2:
#line 111 "parser.y"
{
		  assert_application_name();
		  YACC_DEBUG2(header %s parsed, string);
		}
break;
case 10:
#line 129 "parser.y"
{
                    assert_comp_debug();
                   }
break;
case 12:
#line 135 "parser.y"
{
                     assert_runtime_debug();
                  }
break;
case 13:
#line 140 "parser.y"
{
		    assert_runtime_log();
		  }
break;
case 15:
#line 147 "parser.y"
{
		  YACC_DEBUG(dec_repeat parsed);
		}
break;
case 17:
#line 154 "parser.y"
{
		  YACC_DEBUG(declarations parsed);
		}
break;
case 18:
#line 167 "parser.y"
{
			begin_synonym_proc();
		}
break;
case 19:
#line 171 "parser.y"
{
			assert_synonym_name();
		}
break;
case 20:
#line 175 "parser.y"
{
			assert_synonym_name();
		}
break;
case 21:
#line 179 "parser.y"
{
		  YACC_DEBUG(synonym parsed);
		}
break;
case 22:
#line 185 "parser.y"
{
			assert_synonym_name();
		}
break;
case 24:
#line 192 "parser.y"
{
		  strcpy(quot_string, string);
		}
break;
case 25:
#line 196 "parser.y"
{
		  strcat(quot_string, ".");
		  strcat(quot_string, string);
		  strcpy(string, quot_string);
		}
break;
case 27:
#line 213 "parser.y"
{
		  end_mf_proc();
		}
break;
case 28:
#line 217 "parser.y"
{
		  end_mf_proc();
		}
break;
case 35:
#line 231 "parser.y"
{
			begin_equivs_proc();
			begin_node_proc();
			assert_f();
		}
break;
case 37:
#line 239 "parser.y"
{
			begin_equivs_proc();
			begin_node_proc();
			assert_m(); 
		}
break;
case 39:
#line 250 "parser.y"
{
			end_node_proc();
			begin_node_proc();
		}
break;
case 41:
#line 257 "parser.y"
{
			end_node_proc();
		}
break;
case 42:
#line 271 "parser.y"
{
                    YACC_DEBUG( 2 parts mf_name parsed );
                  }
break;
case 43:
#line 275 "parser.y"
{
                    YACC_DEBUG(1 part mf_name parsed) ; 
                  }
break;
case 44:
#line 280 "parser.y"
{ 
                 assert_l_name();
                }
break;
case 52:
#line 297 "parser.y"
{
		    assert_loc();
		   }
break;
case 53:
#line 301 "parser.y"
{
                    assert_path();
                   }
break;
case 54:
#line 305 "parser.y"
{
                    assert_e_loc();
                   }
break;
case 55:
#line 309 "parser.y"
{
                    assert_e_path(); 
                   }
break;
case 56:
#line 313 "parser.y"
{
                    assert_org();
                   }
break;
case 57:
#line 317 "parser.y"
{
                      assert_parm();
                   }
break;
case 58:
#line 321 "parser.y"
{
		     assert_parmi();
		   }
break;
case 59:
#line 325 "parser.y"
{
                      assert_t();
                   }
break;
case 60:
#line 329 "parser.y"
{
                      assert_fac();
                   }
break;
case 61:
#line 333 "parser.y"
{
                      assert_mode();
                   }
break;
case 62:
#line 337 "parser.y"
{
                      assert_d();
                   }
break;
case 69:
#line 350 "parser.y"
{
			assert_P_name();
		}
break;
#line 739 "y.tab.c"
    }
    yyssp -= yym;
    yystate = *yyssp;
    yyvsp -= yym;
    yym = yylhs[yyn];
    if (yystate == 0 && yym == 0)
    {
#if YYDEBUG
        if (yydebug)
            printf("%sdebug: after reduction, shifting from state 0 to\
 state %d\n", YYPREFIX, YYFINAL);
#endif
        yystate = YYFINAL;
        *++yyssp = YYFINAL;
        *++yyvsp = yyval;
        if (yychar < 0)
        {
            if ((yychar = yylex()) < 0) yychar = 0;
#if YYDEBUG
            if (yydebug)
            {
                yys = 0;
                if (yychar <= YYMAXTOKEN) yys = yyname[yychar];
                if (!yys) yys = "illegal-symbol";
                printf("%sdebug: state %d, reading %d (%s)\n",
                        YYPREFIX, YYFINAL, yychar, yys);
            }
#endif
        }
        if (yychar == 0) goto yyaccept;
        goto yyloop;
    }
    if ((yyn = yygindex[yym]) && (yyn += yystate) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == yystate)
        yystate = yytable[yyn];
    else
        yystate = yydgoto[yym];
#if YYDEBUG
    if (yydebug)
        printf("%sdebug: after reduction, shifting from state %d \
to state %d\n", YYPREFIX, *yyssp, yystate);
#endif
    if (yyssp >= yysslim && yygrowstack())
    {
        goto yyoverflow;
    }
    *++yyssp = yystate;
    *++yyvsp = yyval;
    goto yyloop;

yyoverflow:
    yyerror("yacc stack overflow");

yyabort:
    return (1);

yyaccept:
    return (0);
}
