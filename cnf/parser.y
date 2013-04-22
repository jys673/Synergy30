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

   File: parser.y

   Developers: Avi Freedman, Hasnain Rangwalla, Yuanlong Chang, Yuan Shi 

   Last Change: 6/28/89

*/

%{
extern char application_name[];
extern char integer[1024];
extern char quot_string[1024];
extern char string[1024];

#define YACC_DEBUG(_msg_) if (yacc_debug) printf("\nyyparse: _msg_")
#define YACC_DEBUG2(_m1_, _m2_) if (yacc_debug) printf("\nyyparse: _m1_", _m2_)

int yacc_debug;
%}

%start conf_prog 

%token '.'
%token ','
%token ':'
%token ';'
%token '='
%token '('
%token ')'
%token '+'
%token '['
%token ']'

%token INTEGER
%token QUOT_STRING
%token STRING
 
%token CONFIGURATION
%token C_DEBUG
%token R_DEBUG 
%token R_LOG

%token STORAGE_LOC
%token EXEC_LOC
%token FACTOR
%token THRESHOLD
%token MODE
%token DEBUG 
%token MAX_MSGS
%token PARM

%token NODE_ID

%token OS
%token UNIX
%token MSDOS
%token VMS
%token MACOS

%token SCOLON
%token MCOLON
%token FCOLON
%token ARROW

%token TYPE 
%token TS
%token SEQ
%token PIPE
%token POST
%token DEV 
%token GRP
%token MDL
%token ISAM
%token TERM
%token SCREEN
%token STREAM
%token SLAVE

%token RECSZ
%%

/* 
 * top level productions
 *
 */

conf_prog       : header 
		  csl_body 
		{
		  YACC_DEBUG2("configuration %s parsed", application_name);
		}
		;

header          : CONFIGURATION ':' STRING ';'
		{
		  assert_application_name();
		  YACC_DEBUG2("header %s parsed", string);
		}
		;
csl_body        : debug_switch 
                  dec_repeat
                ;
debug_switch    : debug
                  debug_switch
                | 
                ;                 
debug           : comp_time_debug 
                | run_time_debug 
		| run_time_log 
                ;
comp_time_debug :
                | C_DEBUG '=' INTEGER ';' 
                  {
                    assert_comp_debug();
                   }
                ;
run_time_debug  :
                | R_DEBUG '=' INTEGER ';'
                  {
                     assert_runtime_debug();
                  }
                ;
run_time_log	: R_LOG '=' INTEGER ';'
		  {
		    assert_runtime_log();
		  }
		;

dec_repeat      :
		| dec_repeat declarations 
		{
		  YACC_DEBUG("dec_repeat parsed");
		}
		;

declarations    : synonym 
		| declaration
		{
		  YACC_DEBUG("declarations parsed");
		}
		;

/* end of top level productions */

/*
 * synonym productions
 *
 */

synonym         : SCOLON
		{
			begin_synonym_proc();
		}
		  s_name
		{
			assert_synonym_name();
		}
		  ',' s_name
		{
			assert_synonym_name();
		}
		  s_opt ';'
		{
		  YACC_DEBUG("synonym parsed");
		}
		;

s_opt           : s_opt ',' s_name 
		{
			assert_synonym_name();
		}
		|
		;

s_name		: STRING
		{
		  strcpy(quot_string, string);
		}
		  '.' STRING
		{
		  strcat(quot_string, ".");
		  strcat(quot_string, string);
		  strcpy(string, quot_string);
		}
		| STRING
		;

/* end of synonym productions */


/*
 * declaration productions
 * 
 */

declaration	: f_dec ARROW m_f_dec ';'
		{
		  end_mf_proc();
		}
		| m_f_dec ';'
		{
		  end_mf_proc();
		}
		;
m_f_dec		: m_dec f_dec_opt
		| f_dec f_dec_opt
		;
f_dec_opt	: ARROW f_dec m_dec_opt
		|
		;
m_dec_opt	: ARROW m_dec f_dec_opt
		|
		;
f_dec		: FCOLON
		{
			begin_equivs_proc();
			begin_node_proc();
			assert_f();
		}
		  mf_names
		;
m_dec		: MCOLON
		{
			begin_equivs_proc();
			begin_node_proc();
			assert_m(); 
		}
		  mf_names
		;


mf_names	:
		 mf_name
		{
			end_node_proc();
			begin_node_proc();
		}
		 ',' mf_names
		|
		 mf_name
		{
			end_node_proc();
		}
		;

/* end of declaration productions */


/*
 * path name productions
 * 
 */

mf_name         : part1 part2
                  {
                    YACC_DEBUG(" 2 parts mf_name parsed" );
                  }
                | part1
                  {
                    YACC_DEBUG(" 1 part mf_name parsed" ) ; 
                  }
                ; 
part1           : STRING
                { 
                 assert_l_name();
                }
                  p_name_opt     
                ; 
p_name_opt      : '=' p_name
                |
                ;
part2           : '(' attributes ')'
                ;
attributes      :   attribute
                    attributes
                |
                ;

attribute       : 
                | STORAGE_LOC '=' STRING 
		   {
		    assert_loc();
		   } ':' ':' 
		  STRING  
                   {
                    assert_path();
                   }
                | EXEC_LOC '=' STRING 
                   {
                    assert_e_loc();
                   } ':' ':' 
                  STRING
                   {
                    assert_e_path(); 
                   }
                | TYPE '=' org
                   {
                    assert_org();
                   }
                | PARM '=' STRING  
                   {
                      assert_parm();
                   }
		| PARM '=' INTEGER
		   {
		     assert_parmi();
		   }
                | THRESHOLD '=' INTEGER  
                   {
                      assert_t();
                   }
                | FACTOR '=' INTEGER  
                   {
                      assert_fac();
                   }
                | MODE '=' STRING  
                   {
                      assert_mode();
                   }
                | DEBUG '=' INTEGER  
                   {
                      assert_d();
                   }
                ;

org             : TS | SEQ | PIPE | GRP | MDL | SLAVE; 

/* 
 * low-level productions
 *
 */

p_name		:  STRING
		{
			assert_P_name();
		}
		;

/* end of low-level productions */

%%
