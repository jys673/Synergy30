/*.........................................................................*/
/*             DAC.H ------> Distributed Application Controller            */
/*                   February '13, updated by Justin Y. Shi                */
/*.........................................................................*/

#include "synergy.h"

/* handlers */

#define TSH                      't'
#define FAH                      'f'

/* protocol type - more protocols could be added in the future */
/* a corresponding code must be added in synergy.h */

#define TCP                      "tcp"


/* delimiters in the file */
#define BO_PROC                  "%\n" /* beginning of a new process */
#define BO_HAND                  "@\n" /* beginning of handlers */
#define BO_PRCD                  "-\n" /* beginning of precedence infn */
#define EO_ARGS                  "!\n" /* end of argument list */
#define EO_LINK                  ":\n" /* end of list of links */

/* typedefs */

typedef struct process_struct process_t ;
typedef struct handler_struct handler_t ;
typedef struct prcd_struct prcd_t ;
typedef struct link_struct link_t ;
typedef struct cpulist_struct cpu_t;


/* process */

struct process_struct {
   char name[NAME_LEN] ;	/* process name */
   int proc_id;			/* FSUN 10/94. For FDD to retrieve tuple. */
   char login[LOGIN_LEN] ;	/* login id at execution host */
   char path[PATH_LEN] ;	/* path at execution host */
   u_long host ;		/* execution host */
   long elapsed_time ;		/* process elapsed time YS94 */

   char s_login[LOGIN_LEN] ;	/* login id at storage_host */
   char s_path[PATH_LEN] ;	/* path at storage host */
   u_long S_host ;		/* storage host */

   u_short protocol ;		/* protocol */
   u_short f,p,t,d;		/* parameters */
   u_short arg_cnt ;		/* arguments to the process */
   char args[ARGS_MAX][ARG_LEN] ;

   u_short link_cnt ;
   link_t *link ;		/* linked list of links */

   prcd_t *prcd ;		/* list of precedent processes */

   u_short retflag ;		/* determines if return status expected */
   u_char retstatus ;   	/* exit status of the process */

   u_short state ;		/* current state of the process */
   process_t *next ;
} ;


/* handler */

struct handler_struct {
   char name[NAME_LEN] ;	/* process name */
   int proc_id;			/* FSUN 10/94. For FDD to retrieve tuple. */
   char login[LOGIN_LEN] ;	/* login id at execution host */
   char inet_addr[NAME_LEN];
   u_long host ;		/* execution host */
   char type ;   		/* FAH/TSH */
   u_short port ;               /* port of handler */
   u_short state ;		/* current state of the process */
   char mapid[MAP_LEN];		/* object mapid in debug mode */
   handler_t *next ;
} ;


/* links with a process */

struct link_struct {
   char ref_name[NAME_LEN] ;	/* reference name */
   char obj_name[NAME_LEN] ;	/* object name */
   char path[PATH_LEN] ;	/* only for FILE */
   char mode[MODE_LEN] ;	/* only for FILE */
   char type ;   		/* pipe/file/space */
   char dirn ;	        	/* in/out/nil */
   u_long host ;		/* not for pipe--in */
   char login[NAME_LEN] ;	/* for pipe-out and files */
   link_t *next ;
} ;


/* precedence */

struct prcd_struct {		/* precednce linked list */
   process_t *process ;
   prcd_t *next ;
} ;

struct cpulist_struct {		/* cpu list */
   u_long hostid;
   int proc_id;
   char login[LOGIN_LEN];
   cpu_t *next;
};

cpu_t *cpu_header;


/* global variables */

process_t *process_list ;	/* list of processes */
int plist_cnt;                  /* the length of process list. FSUN 07/94 */
handler_t *handler_list ;	/* list of handlers */
int hlist_cnt;
int fdd_start;			/* FSUN 10/94. For FDD */

char hostname[NAME_LEN];
char app_id[NAME_LEN] ;		/* host-id + pid of dac */
char login[LOGIN_LEN] ;         /* login of the user */
FILE *fp ;                      /* input file for DAC */
u_short ret_port ;		/* return port for contact from CID */
u_short ssh_port;               /* SSH port for DAC_exit_information. FSUN 07/94 */
int oldsock ;			/* sockets to get connection on ret_port */
int newsock ;
int sockcid;			/* socket to send CID the app. status. FSUN 07/94 */
int start_flag ;		/* identifies whether DAC has received */
				/* DAC_OP_START */
int term_flag;                  /* identifies whether DAC should exit by itself */
int cidport ;                   /* remote cid port (reg'd using "host+user") */
char mapid[MAP_LEN];            /* buffer for host+user editing */
char dacmapid[MAP_LEN];         /* buffer for host+user editing */
int debug = 0;
long elapsed_time;
int parent_pid;			/* startDac caller's pid */
int r_debug = 0;			/* runtime debugging support YS94 */
int total_cpu = 0;

/* prototypes */

void readFile(/*char **/) ;
char *getHandlerLogin();

void readProcess(/*void*/) ;
void readLocation(/*process_t **/) ;
void readParams(/*process_t **/) ;
void readArgs(/*process_t **/) ;
void readLinks(/*process_t **/) ;

void readHandler(/*void*/) ;
void addHandlerList(/*void*/) ;

void readPrcd(/*void*/) ;
process_t *getProcess(/*char **/) ;

void updateLinks(/*void*/) ;
u_long getPipeHost(/*char **/) ;
char *getPipeLogin();
u_long getHandlerHost(/*char **/) ;

void initDac(/*void*/) ;
void sigtermHandler(/*void*/) ;
void mapDacport(/*void*/) ;
void unmapDacport(/*void*/) ;

void startExec(/*void*/) ;
void send_cid_appstatus(/*u_char*/);


int inform_tsh_retrieve(/*mapid, host, login*/);


void OpStart(/*void*/) ;
void OpSaveStatus(/*void*/) ;
void OpSaveStatusAll(/*void*/) ;
void OpVerifyApp(/*void*/) ;
void OpExitProcess(/*void*/) ;
void OpGetStatus(/*void*/) ;
void OpGetState(/*void*/) ;
void OpGetAppstate(/*void*/) ;
void OpKill(/*void*/) ;
void OpKillAll(/*void*/) ;
void OpExit(/*void*/) ;
void OpGetProclist(/*void*/);   /* send process list to PCHECK. FSUN 07/94*/
void OpGetHandlerlist(/*void*/);/* send handler list to FDD. FSUN 10/94 */
void OpGetCpulist(/*void*/);    /* send host list to FDD. FSUN 10/94 */
void OpFddChangeProc(/* void*/);/* inform TSH to retrieve the tuple */
                                /* just fetched by a process on     */
                                /* a dead host. FSUN 10/94          */

int verifyProcess(/*process_t **/) ;
int verifyHandler(/*handler_t **/) ;

int execAllHandlers(/*void*/) ;
int sendHandlerToCID(/*int, handler_t **/) ;
int sendToHandler(/*int, handler_t **/) ;
void killAllHandlers(/*void*/) ;

void execReadyProcesses(/*void*/) ;
int sendProcessToCID(/*int, process_t **/) ;
int sendToProcess(/*int, process_t **/) ;
int sendLink(/*int, link_t **/) ;

int modifyPrcd(/*char **/) ;
void killAllProcesses(/*void*/) ;
void killProcess(/*process_t **/) ;
int pingProcess(/*process_t **/) ;
void push_host();
void cleanAllProcesses();
void cleanProcess();
