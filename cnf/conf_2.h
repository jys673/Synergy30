#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "conf.h"		/* data structure templates */

#define ERROR (-1)
#define TRUE 1
#define FALSE 0
#define NOERR 0
#define PARSE_ERR 1		/* error (syntactic) in the input program */
#define CSL_ERR 2		/* error (semantic type) in the input prg */
#define CONF_ERR 3		/* error in the configurator processing */
#define MAX_PROC 20
#define DPC_NODE_NAME "WS5"

extern int lex_debug;		/* in lexan.l, the lex lexical analyser */
extern int yacc_debug;		/* in parser.y, the yacc parser */

/* the following extern declarations are first declared in conf.c */

extern char application_name[128];

extern char integer[1024];
extern char quot_string[1024];
extern char string[1024];

extern struct net_spec *net_nodes;
extern struct net_spec *curr_net_node;

extern struct list_of_syn_lists *synonyms;
extern struct list_of_syn_lists *curr_syn_list;
extern struct syn_node *curr_syn_node;

extern struct node *m_header;
extern struct node *f_header;
extern struct node *curr_m;
extern struct node *curr_f;
extern struct node *curr_temp_node;
extern struct node *multiplex_node;

extern struct lk_list *temp_head;
extern struct lk_list *curr_lk_list;

/* conf level debugger macros */
#define CONF_DBG(_flag_, _msg_) if (_flag_) printf("\nconf: _msg_")
#define CONF_DBG2(_flag_, _m1_, _m2_) if (_flag_) printf("\nconf: _m1_", _m2_)
