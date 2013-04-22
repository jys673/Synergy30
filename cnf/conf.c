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

   File: conf.c

   Desinger  : Yuan Shi 
   Developers: Avi Freedman, Hasnain Rangwalla, Yuan Shi

   Last Change: 8/94 
   Change History:
	o Added node id to internet dotted address translation. YS 7/92
	o Merged *.ini & *.ctl to *.prcd. YS 7/93 
	o Removed dotted address translation to use ~/.sng_hosts only. YS 9/93
	o Added automatic object daemon creation and debugging tools. YS 2/94
	o Changed automatic object creation to use SNGD services. YS 8/94 
	o Added automatic processor assignment. YS 8/94 
	o Changed the debugging tools to avoid the dependency of rsh. YS 8/94
*/

#include <ctype.h>
#include <sys/types.h> 
#include <sys/times.h> 
#include "cnf_sys.h" 
#include "conf.h"		/* data structure declarations */
#include <daemons.h>  		/* Use name server to convert address */ 
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define ERROR (-1)
#define NOERR 0
#define PARSE_ERR 1		/* error (syntactic) in the input prg */
#define CSL_ERR			/* error (semantic type) in the input prg */
#define CONF_ERR 3		/* error in the configurator processing */
extern int lex_debug;		/* in lexan.l, the lexical analyser */
extern int yacc_debug;		/* in parser.y, the yacc parse */
int run_time_debug;             /* 0: no debug, 1: cursory debug, 2:detailed
                                      debug, 3: extensive debug  */
int comp_time_debug;            /* 0-k , k= number of subprograms in CONF */ 
extern FILE *yyin;              /* Dluo interface */
int run_time_log;		/* 0-1, 1=runtime time log */ 
char application_name[MOD_ID_LEN];
				/* name of the distributed application */

char integer[MAX_STR_LEN];	/* digit chars of an integer returned by
				   yylex */
char quot_string[MAX_STR_LEN];	/* chars enclosed in single quotes,
				   returned by yylex */
char string[MAX_STR_LEN];	/* chars of a string returned by yylex */

struct node_cap
        {
	    char ipaddr[PHYS_ID_LEN];    /* IP address */
            char name[PHYS_ID_LEN] ;     /* IP name */ 
	    char user_name[PHYS_ID_LEN]; /* add access user account name */
	    char prot[PHYS_ID_LEN];      /* Protocol: TCP/UDP/DECNET ... */
	    char OS[15];                 /* Unix/VMS/VM ... */
	    u_short used;		/* used mark */
	    char fsys[5];		/* file system mask */
        };
struct node_cap  ary_node_cap[MAXNODES];   /* maximum 500 nodes / apps */ 
static int max_no_procs = 0;

struct net_spec	*nodes = NULL;
struct net_spec	*curr_node = NULL;


struct net_node  *protocol=NULL, *curr_prot= NULL, *dup_node=NULL;

struct net_spec *net_nodes = NULL;	/* head of all net nodes */
struct net_spec *curr_net_node = NULL;

struct list_of_syn_lists *synonyms = NULL;	/* head of all synonyms */
struct list_of_syn_lists *curr_syn_list = NULL;
struct syn_node *curr_syn_node = NULL;

struct node *m_header = NULL;	/* head of all M type nodes */
struct node *f_header = NULL;	/* head of all F type nodes */
struct node *curr_m;
struct node *curr_f;
struct node *curr_temp_node;
struct node *multiplex_node,*tnd;

struct lk_list *temp_head = NULL;	/* head of M F type nodes in one
					   declaration */
struct lk_list *curr_lk_list,*tlk;
struct lk_list *global_slave_ptr = NULL;

#define CONF_DBG(_flag_, _msg_) if (_flag_) printf("\nconf: _msg_")
#define CONF_DBG2(_flag_, _m1_, _m2_) if (_flag_) printf("\nconf: _m1_", _m2_)


char pwd[255];
char dnode[MOD_ID_LEN];
int cnt=0;
struct sockaddr_in dest; 		/* for dotaddress convertion */ 
struct hostent *host; 
char *destdotaddr; 
int re_cycle_no = 0;			/* for recycling cpu list */

char *find_local_name();
char *find_a_local_name();
char *find_target_mail();
char *find_target_e_loc();
char *get_cnf_def_path();
char *get_sender_cpu();
char *get_host_prot();
char *get_local_name(); 
char *username();
char *find_ip_addr();
char *strip_dot();
long gettime(); 
void process_host_names();
char *get_a_cpu();
char *find_host();
void generate_slaves();
void generate_mdls();
int get_cluster_hosts();
void free_rcheck_list();
int redundant();
char *get_a_slave_cpu();
int find_host_idx();
/*----------------------------------------------------------------*/
/* CSL_parse()  Dluo's Interface  */
main(argc, argv)
int argc;
char *argv[];
{
   int yn, i;
   long time0, time1, time2; 
   char filenm[128];

#if !AIX 
   clear_screen();
#endif 
   if (argc < 2) 
   {
	printf("Usage: conf csl_file_name [debug]\n");
	exit(1);
   }
   sprintf(application_name,"%s.csl",argv[1]);

   if ((yyin = fopen(application_name,"r")) == NULL)
   {
	printf("++ Error: CONF failed to open (%s).\n",application_name);
	exit(1);
   }
   nodes = NULL;
   curr_node = NULL;
   protocol=NULL;
   curr_prot= NULL;
   dup_node=NULL;
   net_nodes = NULL;    /* head of all net nodes */
   curr_net_node = NULL;
   synonyms = NULL;     /* head of all synonyms */
   curr_syn_list = NULL;
   curr_syn_node = NULL;
   temp_head = NULL;
   m_header = NULL;
   f_header = NULL;
   if (gethostname(dnode,sizeof(dnode)) == -1)
	printf("++ CONF. Cannot obtain host name??\n");
   for (i=0; dnode[i] != '.' && i < strlen(dnode); i++);
   dnode[i] = 0;
   sprintf(filenm, "%s", dnode);
   // sprintf(filenm, "%s-hps.tc.cornell.edu\0", dnode);
   strcpy(dnode, filenm);
   printf("== Parallel Application Console: (%s)\n", dnode);
   build_net_struct();
   i = find_host_idx(dnode);
   // Get IP address if dnode is in .snghosts
   if (i>=0) strcpy(dnode,ary_node_cap[find_host_idx(dnode)].ipaddr);
   else strcpy(dnode, ary_node_cap[0].ipaddr); // make it the first one
/*
   if (!isdigit(dnode[0]))
   {
     if ((host=gethostbyname(dnode)) == NULL) 
     {	printf("++ CONF. Invalid hostname(%s)\n", dnode); 
      	perror("Invalid Hostname Found");
	exit(1);
     }
     dest.sin_family = host->h_addrtype;
     bcopy(host->h_addr, (caddr_t) & dest.sin_addr, host->h_length);
     destdotaddr = inet_ntoa(dest.sin_addr);
     strcpy(dnode, destdotaddr); 
   }
*/
   printf("== CONFiguring: (%s)\n",application_name);
   getcwd(pwd,255);
   printf("== Default directory: (%s)\n",pwd);
/*
   printf("\n");
   printf(" Synergy Compilation Begin ... \n");
   printf("=========================================================\n");
   printf("    Activity                           Estimated Time    \n");
   printf("=========================================================\n"); 
   time1 = gettime(); 
   time0 = time1; 
*/ 
   lex_debug =  0; 		/* turn off lex analyser debug switch */
   yacc_debug = 0; 		/* turn off yacc parser debug switch */
   
   yn = yyparse();		/* start parsing the input configuration
				   program */
   if (comp_time_debug > 1) show_graph();
/*
   time2 = gettime();
   printf("  Parsing                                %d Sec. \n",time2-time1); 
*/
   if (argc > 2) run_time_debug = 1;
   /* reset parser value */
   sprintf(application_name,"%s",argv[1]);
   if (run_time_debug > 0) printf("++ Debugging mode ON.\n");
   if (comp_time_debug > 0) printf("++ CONF debugging ON. \n");
   if (run_time_log > 0) printf("++ Log ON. \n");
    /* generate time logs without debugging interference */ 
   time1 = gettime(); 
   process_synonyms();
   if (comp_time_debug > 0)
   {
   time2 = gettime();
   printf("  Process Synonyms                       %ld Sec. \n",time2-time1); 
   time1 = gettime(); 
   }
   check_for_errors();
   if (comp_time_debug > 0)
   {
   time2 = gettime();
   printf("  Checking for Errors                    %ld Sec. \n",time2-time1);
   time1 = gettime(); 
   }
   /* generate as many slaves as the processors YS94 */
   generate_slaves();
   if (comp_time_debug > 0)
   {
   time2 = gettime(); 
   printf("  Build Network Structure                %ld Sec. \n",time2-time1);
   time1 = gettime(); 
   }
   calc_phys_names();		/* calculate phys names */
   if (comp_time_debug > 1) show_graph();
   if (comp_time_debug > 0)
   {
   time2 = gettime();
   printf("  Calculate Physical Names               %ld Sec. \n",time2-time1);
   time1 = gettime(); 
   }
   if (run_time_debug == 0) generate_prcd();
   else generate_inis();
   if (comp_time_debug > 0)
   {
   time2 = gettime();
   printf("  Generating IPC data and Control Proc.  %ld Sec. \n",time2-time1);
   time1 = gettime(); 
   generate_net_doc(); 
   time2 = gettime();
   printf("  Generating User System Documentation   %ld Sec. \n",time2-time1);
   printf("===============================================================\n"); 
   printf(" Synergy Compilation End. Total Estimated Time: %ld Sec.\n",time2-time0);
   }
   exit(NOERR);
}
#include "conf.inc"
