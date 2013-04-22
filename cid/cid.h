/*.........................................................................*/
/*                  CID.H ------> Command Interpretter Daemon              */
/*.........................................................................*/
/*              February '13, updated by Justin Y. Shi                     */
 
#include "synergy.h"
#if (SunOs == 0) && (Ultrix == 0) && (EPIX==0) && (Linux == 0)
union wait
{
        int     w_status;               /* used in syscall              */

        struct                          /* terminated process status    */
        {
#if     BYTE_ORDER == BIG_ENDIAN
                unsigned short  w_PAD16;
                unsigned        w_Retcode:8;    /* exit code if w_termsig==0 */
                unsigned        w_Coredump:1;   /* core dump indicator */
                unsigned        w_Termsig:7;    /* termination signal */
#else
                unsigned        w_Termsig:7;    /* termination signal */
                unsigned        w_Coredump:1;   /* core dump indicator */
                unsigned        w_Retcode:8;    /* exit code if w_termsig==0 */
                unsigned short  w_PAD16;
#endif
        } w_T;
        /*
         * Stopped process status.  Returned
         * only for traced children unless requested
         * with the WUNTRACED option bit.
         */
        struct {
#if     BYTE_ORDER == BIG_ENDIAN
                unsigned short  w_PAD16;
                unsigned        w_Stopsig:8;    /* signal that stopped us */
                unsigned        w_Stopval:8;    /* == _WSTOPPED if stopped */
#else
                unsigned        w_Stopval:8;    /* == _WSTOPPED if stopped */
                unsigned        w_Stopsig:8;    /* signal that stopped us */
                unsigned short  w_PAD16;
#endif
        } w_S;
};
#endif

/* constants */

#define INIT_FILE     "cid.conf" /* intialization info - FAH/TSH paths */

                                /* list of synergy users - local logins */
#define USER_FILE     "synergy.user"
#define USER_FILE_TMP "synergy.user.tmp"

#define RLOGIN_FILE   ".sng_rhosts"/* list of rlogin@host for remote access */
                                /* this file present in home dir of each */
                                /* synergy user */

#define OPS_FILE      "/tmp/cidops.log"
#define OPS_FILE_OLD  "/tmp/cidops.old"
#define OPS_MAX       5000      /* max # of operations stored in log file */

#define TABLE_SIZE    200       /* size of the CID table */

#define USED          1         /* entry in CID table is used/unused */
#define UNUSED        0

#define SEGMENT       5000      /* block size for receiving file */

#define STDIN         0         /* stdin fileno. */

/* CID data structure */

typedef struct {
   char appid[NAME_LEN] ;        /* application id */
   char name[NAME_LEN] ;        /* name of component */
   u_short type ;               /* USR/FAH/TSH */
   u_long host ;                /* host, port to be reported on termination */
   sng_int16 port ;
   int pid ;                    /* pid of component */
   int used ;                   /* is the entry in table used/unused */
   int term_stat ;              /* FSUN 10/94. For FDD */
   int fdd_start ;              /* FSUN 10/94. For FDD */
} table_t ;

appmsg_t *list_p, *end_p;
int list_cnt;

table_t table[TABLE_SIZE];              /* CID table */
long start_time ;               /* time when CID was started */

int oldsock ;                   /* socket on which requests are accepted */
int newsock ;                   /* new socket identifying a connection */
u_short this_op ;               /* the current operation that is serviced */
u_long peer ;                   /* address of the requestor */
u_long hostid;                  /* local host id */
sng_int16 Tcidport;             /* local CID port (for local TSH). FSUN 04/96 */
u_short bcr_pid;                    /* Bcast Reciever PID. FSUN 04/96 */
int active_cid;                 /* SIGHANDLER: if active_cid=1, start BCR */

char CID_PATH[PATH_LEN] ;       /* use env string setup YS94 */
char PMD_PATH[PATH_LEN] ;       /* auto-start PMD.  YS94 */
char tsh_path[PATH_LEN] ;       /* location of FAH/TSH executable */
char fah_path[PATH_LEN] ;
char bcr_path[PATH_LEN] ;       /* location of BCR executable. FSUN 04/96 */
char mapid[MAP_LEN];            /* local mapid (host+userid) */
char bcid[BCKEY_LEN];           /* BCast id "sng$bc$"+login. FSUN 04/96 */
                                /* CID/TSH only receive 'login' related bcast*/
int  multiplier;                /* benchmark loop multiplier */
int  fpid;			/* VTget fork pid */
/* Prototypes */

void OpNull(/*void*/) ;
void OpExecTsh(/*void*/) ;
void OpExecFah(/*void*/) ;
void OpExecComponent(/*void*/) ;
void OpKill(/*void*/) ;
void OpKillApp(/*void*/) ;
void OpKillAll(/*void*/) ;
void OpGetApptable(/*void*/) ;
void OpGetFulltable(/*void*/) ;
void OpPing(/*void*/) ;
void OpPingApp(/*void*/) ;
void OpRemove(/*void*/) ;
void OpFilePut(/*void*/) ;
void OpFileGet(/*void*/) ;
void OpAddUser(/*void*/) ;
void OpDeleteUser(/*void*/) ;
void OpVerifyUser(/*void*/) ;
void OpVerifyProcess(/*void*/) ;
void OpVerifyHandler(/*void*/) ;
void OpExit(/*void*/) ;
void OpExitProcess();
void OpResourceCk();
void OpSaveStatus();
void OpGetStatus();
void OpVTUpd();		    /* CID VT Update Op. FSUN 08/96 */
void return_value();

/* V3 FSUN 07/94 */

void OpSaveAppMsg(/*void*/);
void OpChangeAppMsg(/*void*/);
void OpGetAppList(/*void*/);
void OpGetApp(/*void*/);
void OpDelApp(/*void*/);
void OpDelAppList(/*void*/);

void initFromfile(/*void*/) ;
void initSocket(/*void*/) ;
void initOther(/*void*/) ;
void start(/*void*/) ;
int toFork(/*void*/) ;
int doLogin(/*char *, char **/) ;
int validSynergyuser(/*char **/) ;
int validRlogin(/*char **/) ;
int validRhost(/*char **/) ;
int addUser(/*char **/) ;
int deleteUser(/*char **/) ;
int getAppcount(/*char **/) ;
void sendApptable(/*char **/) ;
void sendFulltable(/*void*/) ;
int receiveFile(/*FILE *, long*/) ;
void sendFile(/*FILE *, long*/) ;
int getSlot(/*void*/) ;
void ungetSlot(/*int*/) ;
void sigchldHandler(/*void*/) ;
void sigtermHandler(/*void*/) ;
void sigAlarmHandler();
void logOps() ;
u_short bench();

/* For local TSH. FSUN 04/96 */
void startBCR(/*void*/);               /* Start bcast receiver */
short int BCast_out(/*char *, int*/);  /* Bcast routine */
void VTPut(/*cid_recieve_bc * */);     /* CID/TSH VT put */
void VTGet(/*cid_recieve_bc * */);     /* CID/TSH VT get */

void *initIpc(/*int*/) ;
void exitIpc(/*void*/) ;
void semP(/*void*/) ;
void semV(/*void*/) ;

/*.........................................................................*/
/*                     PMD.H ------> Port Mapper Daemon                    */
/*.........................................................................*/

/* constants */

/* PMD data structure */

typedef struct {
  char mapid[MAP_LEN] ;         /* logical name for port */
  u_short protocol ;            /* tcp/udp */
  sng_int16 port ;                /* port # */
  int pid ;                     /* pid of process that has the port */
  int used ;                    /* is the entry in table used/unused */
} pmd_table_t ;

pmd_table_t *pmd_table ;        /* PMD table */

double second()
{
#include <sys/time.h>
#include <sys/resource.h>
struct timeval tp;
struct timezone tzp;
double t;
        gettimeofday(&tp, &tzp);
        t = (tzp.tz_minuteswest*60 + tp.tv_sec)*1.0e6 +
                (tp.tv_usec)*1.0;
        return t;
}

/*  Prototypes.  */

void OpMap(/*void*/) ;
void OpUnmap(/*void*/) ;
void OpGetMap(/*void*/) ;
void OpGetTable(/*void*/) ;
void OpPurgeTable(/*void*/) ;
void OpDetectProc(/*void*/);
void pmd_OpNull ();

int pmd_getSlot(/*char *, u_short*/) ;
int unmapMapid(/*char *, u_short*/) ;
int unmapPid(/*u_long*/) ;
void sendTable(/*void*/) ;
void pmd_logOps(/*void*/) ;
void kill_old_cid();

/*.........................................................................*/
/*                     TSH.H ------> Tuple Space Handler                   */
/*.........................................................................*/

/*  Tuples data structure.  */

struct t_space1 {
   char appid[NAME_LEN] ;        /* application id. FSUN 04/96 */
   char name[TUPLENAME_LEN] ;   /* tuple name */
   char *tuple ;                /* pointer to tuple */
   u_short priority ;           /* priority of the tuple */
   u_long length ;              /* length of tuple */
   struct t_space1 *next ;
   struct t_space1 *prev ;
} ;
typedef struct t_space1 space1_t ;

/*  Backup tuple list. FSUN 09/94 */
/*  host1(tp) -> host2(tp) -> ... */
struct t_space2 {
   char appid[NAME_LEN] ;        /* application id. FSUN 04/96 */
   char name[TUPLENAME_LEN];
   char *tuple;
   u_short priority;
   u_long length;
   u_long host;
   sng_int16 port;
   int proc_id;
   int fault;
   struct t_space2 *next;
};
typedef struct t_space2 space2_t;

/*  Pending requests data structure.  */

struct t_queue {
   char appid[NAME_LEN] ;        /* application id. FSUN 04/96 */
   char expr[TUPLENAME_LEN] ;   /* tuple name */
   u_long host ;                /* host from where the request came */
   sng_int16 port ;               /* corresponding port # */
   u_short request ;            /* read/get */
   struct t_queue *next ;
   struct t_queue *prev ;
} ;
typedef struct t_queue queue1_t ;

/*  Tuple space data structure.  */
/*  Change the structure of tuple space to TWO levels. FSUN 09/96 */

struct t_apptq {		/* A medium level is inserted between */
				/* Tuple Space and Tuples, on which */
				/* tuples are grouped into Tuple Queues */
				/* by APPID.*/
   char appid[NAME_LEN] ;       
   space1_t *Tqueue ;            /* list of tuples of a particular app.*/
   struct t_apptq *next ;
   struct t_apptq *prev ;
} ;
typedef struct t_apptq apptq_t ;

struct t_apprq {
   char appid[NAME_LEN] ;
   queue1_t *queue_hd ;        /* queue of waiting requests */
   queue1_t *queue_tl ;        /* new requests added at the end */
   struct t_apprq *next ;
   struct t_apprq *prev ;
} ;
typedef struct t_apprq apprq_t ;

struct {
   apptq_t *space ;            /* list of tuple queues. FSUN 09/96 */
   space2_t *retrieve ;        /* list of tuples retrieved. FSUN 09/94 */
   apprq_t *request ;          /* list of request queues. FSUN 09/96 */
} tsh ; 

/*  Prototypes.  */

void OpPut(/*void*/) ;
void OpGet(/*void*/) ;
void OpRetrieve(/*void*/);
void OpTid();
void OpZap();

space1_t *createTuple(/*char *, char *, u_long, u_short*/) ;
int consumeTuple(/*space1_t **/) ;
short int storeTuple(/*space1_t **/) ;
space1_t *findTuple(/*char *, char *, int*/) ;
void deleteTuple(/*space1_t **/) ;
int sendTuple(/*queue_t *, space1_t **/) ;
queue1_t *findRequest(/*char *, char **/) ;
int storeRequest(/*char *, char *, u_long, u_short*/) ;
void deleteRequest(/*queue_t *q*/) ;
int match(/*char *, char **/) ;
int guardf(/*u_long hostid*/); 
int DelTRQUE(/*char **/);

