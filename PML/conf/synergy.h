/*.........................................................................*/
/*                     SYNERGY.H ------> Common header file                */
/*.........................................................................*/

#include "portab.h"
#include <stdio.h>              /* fopen, fclose, fgets, fwrite, printf */
                                /* sprintf*/
#include <sys/file.h>           /* open, access */
#include <errno.h>              /* errno, EINTR */
#include <string.h>             /* strlen, strchr, strcpy */
#include <malloc.h>             /* malloc */
#include <sys/types.h>          /* socket, bind, accept, getpid, kill, signal*/
                                /* htons, htonl, ntohs, ntohl, writev, signal*/
#include <sys/stat.h>           /* stat */
#include <signal.h>             /* signal */
#include <pwd.h>                /* getpwnam */
#include <sys/socket.h>         /* socket, bind, accept, inet_addr */
#include <netinet/in.h>         /* htons, htonl, ntohs, ntohl, inet_addr */
#include <arpa/inet.h>          /* inet_addr */
#if OSF1
#include <netinet/tcp.h>        /* setsockopt */
#endif 
#include <netdb.h>              /* gethostname, gethostbyname */
#include <sys/wait.h>           /* wait3 */
#include <sys/time.h>           /* wait3 */
#include <sys/resource.h>       /* wait3 */
#include <unistd.h>             /* alarm */
#include <ctype.h>              /* isdigit */
#if (Ultrix || IRIX || Solaris || AIX || EPIX)
#include <rpc/types.h>
#endif

u_long sng_gethostid() ;

                                /* routines from netlib.c */
int get_socket(/*void*/) ;
u_short bind_socket(/*int, u_short*/) ;
int get_connection(/*int, u_long **/) ;
int do_connect(/*int, u_long, u_short*/) ;
int writen(/*int, char *, int*/) ;
int readn(/*int, char *, int*/) ;

                                /* routines from pmdlib.c */
int pmd_unmap(/*char *, int, u_short*/) ;
int pmd_map(/*char *, u_short, u_short*/) ;
int pmd_getmap(/*char *, u_long, u_short*/) ;
int ping_pmd(/*u_long*/) ;

                                /* routines from cidlib.c */
int ping_cid(/*char **/) ;
int add_user(/*char *, char **/) ;
int delete_user(/*char *, char **/) ;
int verify_user(/*char *, char **/) ;
int file_put(/*char *, char *, char *, char **/) ;
int file_get(/*char *, char *, char *, char **/) ;
int file_remove(/*char *, char *, char **/) ;

                                /* routines from daclib.c */
int start_dac(/*u_short *, char **/) ;
int start_app(/*u_short*/) ;
int save_process_status(/*u_short, char **/) ;
int save_all_status(/*u_short*/) ;
int verify_app(/*u_short, char **/) ;
int get_process_status(/*u_short, char *, u_short *, u_char **/) ;
int get_process_state(/*u_short, char *, u_short **/) ;
int get_app_state(/*u_short, u_short **/) ;
int kill_process(/*u_short, char **/) ;
int exit_app(/*u_short*/) ;
int exit_dac(/*u_short*/) ;

/* constants */

#define NORMAL_EXIT         179
#define ABNORMAL_EXIT      -179
#define FDD_ALIVE           1

#define TRUE                1
#define FALSE               0

#define YES                 1
#define NO                  0
#define SUCCESS             1
#define FAILURE             0
#define VTPUT_FLAG	    1   /* VT PUT operation. FSUN 08/96 */
#define VTGET_FLAG	    0   /* VT GET operation. FSUN 08/96 */

#define TIME$OUT            3

#define NAME_LEN            64  /* name of component */

/*  #define APP_LEN             128  name of application */
#define APP_LEN             8  /*name of application */

#define ARG_LEN             32  /* length of an argument */
#define ARGS_MAX            10  /* max # of arguments per component */
#define PATH_LEN            255 /* path of file */
#define LOGIN_LEN           9   /* length of login id */
                                /* length of names in PMD table */
#define MAP_LEN             (2*(NAME_LEN+APP_LEN))
#define MODE_LEN            5   /* file open modes */

/*  #define TUPLENAME_LEN       32   name of a tuple */
#define TUPLENAME_LEN       128  /* name of a tuple */

#define BUFFE_SIZE          50  /* V3 FSUN 07/94 */
#define BCKEY_LEN           32  /* Length of BCast ID. FSUN 04/96 */
#define BCQ_LEN             1024/* Length of BCast Queue FSUN 08/96 */

#define SERV_UDP_PORT   16000
#define SERV_TCP_PORT   16000

/*
  CID specific data
*/

#define CID_PORT                  5555

#define CID_OP_MIN                101
#define CID_OP_MAX                130
#define CID_OP_CNT                30

#define CID_OP_UNKNOWN            100
#define CID_OP_NULL               101
#define CID_OP_EXEC_TSH           102
#define CID_OP_EXEC_FAH           103
#define CID_OP_EXEC_COMPONENT     104
#define CID_OP_KILL               105
#define CID_OP_KILL_APP           106
#define CID_OP_KILL_ALL           107
#define CID_OP_GET_APPTABLE       108
#define CID_OP_GET_FULLTABLE      109
#define CID_OP_PING               110
#define CID_OP_PING_APP           111
#define CID_OP_REMOVE             112
#define CID_OP_FILE_PUT           113
#define CID_OP_FILE_GET           114
#define CID_OP_ADD_USER           115
#define CID_OP_DELETE_USER        116
#define CID_OP_VERIFY_USER        117
#define CID_OP_VERIFY_PROCESS     118
#define CID_OP_VERIFY_HANDLER     119
#define CID_OP_EXIT               120
#define CID_OP_EXIT_PROCESS       121
#define CID_OP_RESOURCE_CK        122
#define CID_OP_SAVE_APPMSG        123
#define CID_OP_CHANG_APPMSG       124
#define CID_OP_GET_APPLIST        125
#define CID_OP_GET_APP            126
#define CID_OP_DEL_APPLIST        127
#define CID_OP_DEL_APP            128
#define CID_OP_DETECT_PROC        129
#define CID_OP_VT_UPD		  130

#define CID_ER_NOERROR            100
#define CID_ER_NOENTRY            101
#define CID_ER_NOSPACE            102
#define CID_ER_NOEXEC             103
#define CID_ER_NOACCESS           104
#define CID_ER_NOLOGIN            105
#define CID_ER_NOREMOVE           106
#define CID_ER_NOTRANSFER         107
#define CID_ER_NOADD              108
#define CID_ER_NODELETE           109

#define CID_TYPE_USR              100
#define CID_TYPE_TSH              101
#define CID_TYPE_FAH              102
#define CID_TYPE_BCR              103 /* Bcast receiver type sub-proc. */

typedef struct {
   char login[LOGIN_LEN] ;
   char rlogin[LOGIN_LEN] ;
   char appid[NAME_LEN] ;
   char name[NAME_LEN] ;
   sng_int16 r_debug ;          /* record the debugging info. YS94 */
   int fdd_start;               /* FSUN 10/94. For FDD */
} cid_exectsh_it ;

typedef struct {
   char login[LOGIN_LEN] ;
   char rlogin[LOGIN_LEN] ;
   char appid[NAME_LEN] ;
   char name[NAME_LEN] ;
   char path[PATH_LEN] ;
   char args[ARGS_MAX][ARG_LEN] ;
   sng_int16 arg_cnt ;
   sng_int16 port ;             /* received in NW order, retain NW order */
   int fdd_start;               /* FSUN 10/94. For FDD */
} cid_execcomponent_it ;

typedef struct {
   sng_int16 status ;
   sng_int16 error ;
   int proc_id;
} cid_exectsh_ot, cid_execcomponent_ot;

typedef struct {
   char login[LOGIN_LEN] ;
   char rlogin[LOGIN_LEN] ;
   char path[PATH_LEN] ;
} cid_remove_it ;

typedef struct {
   char login[LOGIN_LEN] ;
   char rlogin[LOGIN_LEN] ;
   char path[PATH_LEN] ;
   sng_ints32 len ;
} cid_fileput_it ;

typedef struct {
   char login[LOGIN_LEN] ;
   char rlogin[LOGIN_LEN] ;
   char path[PATH_LEN] ;
} cid_fileget_it ;

typedef struct {
   char appid[NAME_LEN] ;
   char name[NAME_LEN] ;
} cid_kill_it, cid_ping_it ;

typedef struct {
   char appid[NAME_LEN];
   int proc_id;
} cid_detectproc_it;

typedef struct {
   char appid[NAME_LEN] ;
} cid_killapp_it, cid_pingapp_it, cid_getapptable_it ;

typedef struct {
   char login[LOGIN_LEN] ;
} cid_adduser_it, cid_deleteuser_it, cid_verifyuser_it ;

typedef struct {
   char login[LOGIN_LEN] ;
   char rlogin[LOGIN_LEN] ;
   char path[PATH_LEN] ;
} cid_verifyprocess_it ;

typedef struct {
   char login[LOGIN_LEN] ;
   char rlogin[LOGIN_LEN] ;
   sng_int16 type ;
} cid_verifyhandler_it ;

typedef struct {
   sng_int16 status ;
   sng_int16 error ;
   sng_ints64 start_time ;
} cid_null_ot ;

typedef struct {
   sng_int16 status ;
   sng_int16 error ;
} cid_kill_ot, cid_exit_ot, cid_ping_ot, cid_remove_ot, cid_fileput_ot,
   cid_adduser_ot, cid_deleteuser_ot, cid_verifyuser_ot, cid_detectproc_ot,
   cid_verifyprocess_ot, cid_verifyhandler_ot, cid_exit_process_ot ;

typedef struct {
   sng_int16 status ;
   sng_int16 error ;
   sng_ints32 len ;
} cid_fileget_ot ;

typedef struct {
   sng_int16 status ;
   sng_int16 error ;
   sng_int16 count ;
} cid_killapp_ot, cid_killall_ot, cid_pingapp_ot, cid_getapptable_ot1,
   cid_getfulltable_ot1 ;

typedef struct {
   char name[NAME_LEN] ;
   sng_int16 type ;
} cid_getapptable_ot2 ;

typedef struct {
   char appid[NAME_LEN] ;
   char name[NAME_LEN] ;
   sng_int16 type ;
} cid_getfulltable_ot2 ;

typedef struct {
   char app_id[MAP_LEN];
   sng_int16 dacport;
   u_char app_status;
   int num_process;
   char start_time[128];
   char complete_time[128];
} cid_saveappmsg_it, cid_getapp_ot;

typedef struct {
   char app_id[MAP_LEN];
   u_char app_status;
   long elapsed_time;
} cid_changeappmsg_it;

typedef struct {
   int list_cnt;
   cid_saveappmsg_it buffer[BUFFE_SIZE];
} cid_getapplist_ot;

/*  Data structure for Broadcast. FSUN 04/96 */
typedef struct {
   char bckey[BCKEY_LEN];
   char appid[NAME_LEN];
   char name[TUPLENAME_LEN];
   int priority;
   sng_int32 host;
   sng_int16 port;
   int opflag;              /* Get or Put flag */
} cid_send_bc, cid_recieve_bc;

typedef struct appmsg_struct appmsg_t;
struct appmsg_struct {
   char app_id[NAME_LEN];
   sng_int16 dacport;
   u_char app_status;
   int num_process;
   char start_time[128];
   char complete_time[128];
   long elapsed_time;
   int flag;
   appmsg_t *next;
};


/*
  PMD specific data
*/

#define PMD_PORT                  6666

#define PMD_OP_MIN                201
#define PMD_OP_MAX                207
#define PMD_OP_CNT                7

#define PMD_OP_UNKNOWN            200
#define PMD_OP_NULL               201
#define PMD_OP_MAP                202
#define PMD_OP_UNMAP              203
#define PMD_OP_GET_MAP            204
#define PMD_OP_GET_TABLE          205
#define PMD_OP_PURGE_TABLE        206
#define PMD_OP_EXIT               207

#define PMD_ER_NOERROR            200
#define PMD_ER_NOENTRY            201
#define PMD_ER_NOSPACE            202

#define PMD_PROT_TCP              200
#define PMD_PROT_UDP              201

typedef struct {
  char mapid[MAP_LEN] ;
  int pid ;
  sng_int16 port ;
  sng_int16 protocol ;
} pmd_map_it ;

typedef struct {
  char mapid[MAP_LEN] ;
  int pid ;
  sng_int16 protocol ;
} pmd_unmap_it ;

typedef struct {
  char mapid[MAP_LEN] ;
  sng_int16 protocol ;
} pmd_getmap_it ;

typedef struct {
  sng_int16 status ;
  sng_int16 error ;
  sng_int16 count ;
  sng_ints64 start_time ;
} pmd_null_ot ;

typedef struct {
  sng_int16 status ;
  sng_int16 error ;
  sng_int16 count ;
} pmd_purgetable_ot, pmd_unmap_ot, pmd_gettable_ot1 ;

typedef struct {
  sng_int16 status ;
  sng_int16 error ;
} pmd_map_ot, pmd_exit_ot ;

typedef struct {
  sng_int16 status ;
  sng_int16 error ;
  sng_int16 port ;
} pmd_getmap_ot ;

typedef struct {
  char mapid[MAP_LEN] ;
  sng_int16 protocol ;
  sng_int16 port ;
} pmd_gettable_ot2 ;


/*
  FAH specific data
*/

#define FAH_OP_MIN                301
#define FAH_OP_MAX                311
#define FAH_OP_CNT                11

#define FAH_OP_FOPEN              301
#define FAH_OP_FCLOSE             302
#define FAH_OP_FGETC              303
#define FAH_OP_FPUTC              304
#define FAH_OP_FGETS              305
#define FAH_OP_FPUTS              306
#define FAH_OP_FREAD              307
#define FAH_OP_FWRITE             308
#define FAH_OP_FSEEK              309
#define FAH_OP_FFLUSH             310
#define FAH_OP_EXIT               311

#define FAH_ER_NOERROR            300
#define FAH_ER_FOPEN              301
#define FAH_ER_FCLOSE             302
#define FAH_ER_FGETC              303
#define FAH_ER_FPUTC              304
#define FAH_ER_FGETS              305
#define FAH_ER_FPUTS              306
#define FAH_ER_FREAD              307
#define FAH_ER_FWRITE             308
#define FAH_ER_FSEEK              309
#define FAH_ER_FFLUSH             310
#define FAH_ER_INSTALL            311
#define FAH_ER_NOSPACE            312
#define FAH_ER_NOTOPEN            313


typedef struct {
   char appid[NAME_LEN] ;
   char name[NAME_LEN] ;
} fah_start_it ;

typedef struct {
   char path[PATH_LEN] ;
   char mode[MODE_LEN] ;
} fah_fopen_it ;

typedef struct {
   sng_int16 fid ;
} fah_fclose_it, fah_fflush_it, fah_fgetc_it ;

typedef struct {
   sng_int16 fid ;
   char ch ;
} fah_fputc_it ;

typedef struct {
   sng_int16 fid ;
   int len ;
} fah_fgets_it, fah_fputs_it ;

typedef struct {
   sng_int16 fid ;
   int size ;
   int nitems ;
} fah_fwrite_it, fah_fread_it ;

typedef struct {
   sng_int16 fid ;
   sng_ints32 offset ;
   int from ;
} fah_fseek_it ;

typedef struct {
   sng_int16 status ;
   sng_int16 error ;
   sng_int16 port ;
} fah_start_ot ;

typedef struct {
   sng_int16 fid ;
   sng_int16 status ;
   sng_int16 error ;
} fah_fopen_ot ;

typedef struct {
   sng_int16 status ;
   sng_int16 error ;
} fah_fclose_ot, fah_fflush_ot, fah_fputc_ot, fah_fputs_ot, fah_fseek_ot,
   fah_exit_ot ;

typedef struct {
   sng_int16 status ;
   sng_int16 error ;
   int ch ;
} fah_fgetc_ot ;

typedef struct {
   sng_int16 status ;
   sng_int16 error ;
   int len ;
} fah_fgets_ot ;

typedef struct {
  sng_int16 status ;
  sng_int16 error ;
  int nitems ;
} fah_fwrite_ot, fah_fread_ot ;


/*
  TSH specific data
*/

#define TSH_OP_MIN                401
#define TSH_OP_MAX                415
#define TSH_OP_CNT                15 

#define TSH_OP_PUT                401
#define TSH_OP_GET                402
#define TSH_OP_READ               403
#define TSH_OP_EXIT               404
#define TSH_OP_RETRIEVE           405
#define TSH_OP_TIDINFO            406   /* get tid host & cid_port */
#define TSH_OP_SOT                407   /* marks start of scater */
#define TSH_OP_EOT                408   /* marks end of scater */
#define TSH_ADVANCED_OP_ACTIVATE  409   /* delete terminator by worker ID */
#define TSH_ADVANCED_OP_CLEANUP_QUEUE 410 /* clean up Queue by worker ID */
#define TSH_ADVANCED_OP_CLEANUP_SPACE 411 /* clean up Space by worker ID */
#define TSH_ADVANCED_OP_SEND_DATA 412   /* compiler-generated master/workers */
#define TSH_ADVANCED_OP_READ_DATA 413   /* compiler-generated master/workers */
#define TSH_ADVANCED_OP_SET_TOKEN 414   /* compiler-generated master/workers */
#define TSH_ADVANCED_OP_GET_TOKEN 415   /* compiler-generated master/workers */

#define TSH_ER_NOERROR            400
#define TSH_ER_INSTALL            401
#define TSH_ER_NOTUPLE            402
#define TSH_ER_NOMEM              403
#define TSH_ER_OVERRT             404
#define TSH_ER_NOBCAST            405   /* Bcast error */

typedef struct {
  char appid[NAME_LEN] ;
  char name[TUPLENAME_LEN] ;
} tsh_start_it ;

typedef struct {
  char appid[NAME_LEN] ;
  char name[TUPLENAME_LEN] ;
  sng_int16 priority ;
  sng_int32 host ;
  sng_int16 port ;
  sng_int32 length ;
  int proc_id;
} tsh_put_it ;

typedef struct {
  char appid[NAME_LEN] ;
  char expr[TUPLENAME_LEN] ;
  sng_int32 host ;
  sng_int16 port ;
  sng_int32 len  ;
  int proc_id;
  sng_int16 cidport;  /* local cid port for tid info */
} tsh_get_it;

typedef struct {
  sng_int32 host;
  int proc_id;
} tsh_retrieve_it;

typedef struct {
  char tpname[TUPLENAME_LEN] ;
} tsh_tidinfo_it;

typedef struct {
  int status; 		/* 0: not available. 1: available */
  char tid[NAME_LEN];
  sng_int32 tid_host;
  sng_int16 tid_cid_port;
} tsh_tidinfo_ot;

typedef struct {
  sng_int16 status ;
  sng_int16 error ;
  sng_int16 port ;
} tsh_start_ot ;

typedef struct {
  sng_int16 status ;
  sng_int16 error ;
} tsh_put_ot, tsh_get_ot1, tsh_exit_ot, tsh_retrieve_ot ;

typedef struct {
  char appid[NAME_LEN] ;
  char name[TUPLENAME_LEN] ;
  sng_int32 length ;
  sng_int16 priority ;
} tsh_get_ot2 ;


/*
  DAC, component specific data
*/

#define DAC_OP_MIN               501
#define DAC_OP_MAX               515
#define DAC_OP_CNT               15

#define DAC_OP_START             501 /* start executing processes/handlers */
#define DAC_OP_SAVE_STATUS       502 /* save exit-status of the process */
#define DAC_OP_SAVE_STATUS_ALL   503 /* save exit-status of all processes */
#define DAC_OP_VERIFY_APP        504 /* check whether executables available */
#define DAC_OP_EXIT_PROCESS      505 /* process has exitted */
#define DAC_OP_GET_STATUS        506 /* get exit-status of process */
#define DAC_OP_GET_STATE         507 /* get state of process */
#define DAC_OP_GET_APPSTATE      508 /* get state of the application */
#define DAC_OP_KILL              509 /* kill a process */
#define DAC_OP_KILL_ALL          510 /* kill all processes */
#define DAC_OP_FDDCHANGE_PROC    511 /* retrieve tuple. FSUN 10/94 */
#define DAC_OP_GETPROCLIST       512 /* get the process list. FSUN 07/94 */
#define DAC_OP_GETHANDLERLIST    513 /* get the handler list. FSUN 10/94 */
#define DAC_OP_GET_CPULIST       514 /* get the cpu list. FSUN 10/94 */
#define DAC_OP_EXIT              515 /* kill all processes/handlers & exit */

/* errors on op-codes */

#define DAC_ER_NOERROR           500 /* no error in this operation */
#define DAC_ER_NOSTATUS          501 /* retstatus cannot be got for process */
#define DAC_ER_NOPROCESS         502 /* no process by the specified name */
#define DAC_ER_EXECHANDLER       503 /* error in executing handler */
#define DAC_ER_NOTSTARTED        504 /* dac not started yet */
#define DAC_ER_STARTED           505 /* dac already started */
#define DAC_ER_EXECPROCESS       506 /* cannot verify application */


/* exit errors */

#define E_ARGCNT               -1 /* command line arguments - wrong */
#define E_FILEOPEN             -2 /* unable to open .prcd file */
#define E_APPNAME              -3 /* application name - read error */
#define E_DELIMITER            -4 /* incorrect delimiter in .prcd */
#define E_MALLOC               -5 /* error in memory allocation */
#define E_PROCNAME             -6 /* error in reading processs name */
#define E_STORAGELOC           -7 /* error in reading storage location */
#define E_EXECLOC              -8 /* error in reading execution location */
#define E_PROTOCOL             -9 /* error in reading protocol */
#define E_PARAMS              -10 /* error in reading f, p, t, d parameters */
#define E_EOF                 -11 /* unexpected EOF */
#define E_MOREARGS            -12 /* more arguments to process than limit */
#define E_LINK                -13 /* error in reading link infn. */
#define E_HANDLER             -14 /* error in reading handler infn. */
#define E_LINKHOST            -15 /* error in obtaining host for a link */
#define E_PRCD                -16 /* error in reading precedence */
#define E_NOPORT              -17 /* unable to obtain return port */
#define E_NOMAP               -18 /* could not map with PMD */
#define E_LOGIN               -19 /* could not obtain login name */
#define E_CONNECT             -20 /* error getting connection */
#define E_SIGNAL              -21 /* error in setting up SIGTERM handler */
#define E_SOCKET              -22 /* error in getting socket */

/* link object-types */

#define O_PIPE                   'p'
#define O_SPACE                  't'
#define O_FILE                   'f'


/* direction of link with the object, NIL for file and tuple space */

#define IN                       '<'
#define OUT                      '>'
#define NIL                      '*'


/* protocol types, more protocols could be added in the future */

#define DAC_PROT_TCP             0


/* application state */

#define COMPLETED                0
#define NOT_COMPLETED            1

/* V3 FSUN 07/94 */

#define APP_READY                'r'
#define APP_RUNNING              'R'
#define APP_EXITTED              'X'
#define APP_KILLED               'K'
#define APP_NOT_READY            'N'
#define APP_PROC_NOT_READY       'p'
#define APP_OBJ_NOT_READY        'o'


/* state of the process */

#define WAITING                  0 /* requires precedent to be completed */
#define READY                    1 /* can be started  */
#define RUNNING                  2 /* is running currently */
#define STARTED                  3 /* started, no idea what happened */
#define EXITTED                  4 /* has exitted */
#define FAILED                   5 /* exec failed */
#define KILLED                   6 /* killed by user */

typedef struct {
   char name[NAME_LEN] ;
} dac_savestatus_it, dac_getstatus_it, dac_kill_it, dac_getstate_it ;

typedef struct {
   char name[NAME_LEN] ;
   sng_int16 retstatus ;
} dac_exitprocess_it ;

typedef struct {
   sng_int16 status ;
   sng_int16 error ;
} dac_start_ot, dac_savestatus_ot, dac_savestatusall_ot, dac_kill_ot,
   dac_killall_ot, dac_exit_ot, dac_retrievetuple_ot ;

typedef struct {
   sng_int16 status ;
   sng_int16 error ;
   char name[NAME_LEN] ;
} dac_verifyapp_ot ;

typedef struct {
   sng_int16 status ;
   sng_int16 error ;
   sng_int16 state ;
} dac_getstate_ot ;

typedef struct {
   sng_int16 status ;
   sng_int16 error ;
   sng_int16 state ;
   sng_int16 retstatus ;                /* valid only if state = EXITTED */
} dac_getstatus_ot ;

typedef struct {
   sng_int16 status ;
   sng_int16 error ;
   sng_int16 state ;
} dac_getappstate_ot ;


/* commn. with process */

typedef struct {
   sng_int32 host;		/* process host id. from dac */
   char name[NAME_LEN] ;
   char appid[NAME_LEN] ;
   sng_int16 protocol ;
   sng_int16 f ;
   sng_int16 p ;
   sng_int16 t ;
   sng_int16 d ;
   sng_int16 link_cnt ;
} dac_data_ot ;

typedef struct {
   char type ;                  /* pipe/file/space */
   char dirn ;                  /* in/out/nil */
   char ref_name[NAME_LEN] ;    /* reference name */
   char obj_name[NAME_LEN] ;    /* object name */
   sng_int32 host ;                     /* not for input PIPE */
   char path[PATH_LEN] ;        /* only for FILE */
   char mode[MODE_LEN] ;        /* only for FILE */
   char login[PATH_LEN] ;       /* remote login id YS 94 */
} dac_link_ot ;

typedef struct {
   sng_int16 status ;
   sng_int16 error ;
} dac_data_it ;

typedef struct {
   int plist_cnt;
   struct proc_struct {
        char name[NAME_LEN] ;        /* process name */
        char login[LOGIN_LEN] ;      /* login id at execution host */
        char path[PATH_LEN] ;        /* path at execution host */
        sng_int32 host ;                 /* execution host */
        sng_int16 state ;                /* current state of the process */
        char elapsed_time[128];      /* elapsed time */
   } buffer[BUFFE_SIZE];
} dac_getproclist_ot;


typedef struct procinfo_struct {
        char name[NAME_LEN] ;        /* process name */
        char login[LOGIN_LEN] ;      /* login id at execution host */
        char path[PATH_LEN] ;        /* path at execution host */
        sng_int32 host ;                 /* execution host */
        sng_int16 state ;                /* current state of the process */
        char elapsed_time[128];           /* elapsed time */
        struct procinfo_struct *next;
} procinfo_t;

typedef struct {
        int hlist_cnt;
        struct hand_struct {
                char name[NAME_LEN];
                char login[LOGIN_LEN];
                char mapid[MAP_LEN];
                char inet_addr[NAME_LEN];
                sng_int32 host;
                sng_int16 port;
                char type;
        } buffer[BUFFE_SIZE];
} dac_gethandlerlist_ot;

typedef struct handlerinfo_struct {
        char name[NAME_LEN];
        char login[LOGIN_LEN];
        char mapid[MAP_LEN];
        char inet_addr[NAME_LEN];
        sng_int32 host;
        sng_int16 port;
        char type;
        struct handlerinfo_struct *next;
} handlerinfo_t;

typedef struct {
   int hlist_cnt;
   struct cpu_struct {
        char app_id[APP_LEN];
        sng_int64 hostid;
        int proc_id;
        char login[LOGIN_LEN];
   } buffer[BUFFE_SIZE];
} dac_getcpulist_ot;

typedef struct host_struct {
   char app_id[APP_LEN];
   sng_int64 hostid;
   int proc_id;
   char login[LOGIN_LEN];
   sng_int16 cidport;
   int fault_flag;
   struct host_struct *next;
} host_t;

typedef struct {
   sng_int64 hostid;
   int proc_id;
   char login[LOGIN_LEN];
   sng_int16 cidport;
} dac_fddchangeproc_it;
