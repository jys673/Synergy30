/*.........................................................................*/
/*                     TSH.H ------> Tuple Space Handler                   */
/*.........................................................................*/

#include "synergy.h"

/*  Tuples data structure.  */

struct t_space1 {
   char name[TUPLENAME_LEN] ;   /* tuple name */
   char *tuple ;                /* pointer to tuple */
   u_short priority ;           /* priority of the tuple */
   u_long length ;              /* length of tuple */
   /* ------For Advanced Operation --------------------- */
   int tokens ;
   int scalar ;
   /*----------------------------------------------------*/
   struct t_space1 *next ;
   struct t_space1 *prev ;
} ;
typedef struct t_space1 space1_t ;

/*  Backup tuple list. FSUN 09/94 */
/*  host1(tp) -> host2(tp) -> ... */
struct t_space2 {
        char name[TUPLENAME_LEN];
        char *tuple;
        u_short priority;
        u_long length;
        u_long host;
        u_short port;
        u_short cidport;  /* for dspace. ys'96 */
        int proc_id;
        int fault;
        struct t_space2 *next;
};
typedef struct t_space2 space2_t;

/*  Pending requests data structure.  */

struct t_queue {
   char expr[TUPLENAME_LEN] ;   /* tuple name */
   u_long host ;                /* host from where the request came */
   u_short port ;               /* corresponding port # */
   u_short cidport ;            /* for dspace. ys'96 */
   int proc_id;                 /* FSUN 10/94. For FDD */
   u_short request ;            /* read/get */
   /* ------For Advanced Operation --------------------- */
   char *data ;                 /* pointer to tuple */
   int full_to_full ;           /* flag for full tuple to full req. */
   u_short priority ;           /* priority of the tuple */
   u_long reqLen ;              /* request length */
   u_long curLen ;              /* current length */
   /*----------------------------------------------------*/
   struct t_queue *next ;
   struct t_queue *prev ;
} ;
typedef struct t_queue queue1_t ;

/*  Tuple space data structure.  */

struct {
   char appid[NAME_LEN] ;       /* application id */
   char name[NAME_LEN] ;        /* name of the tuple space */
   u_short port ;               /* port where it receives commands */

   space1_t *space ;            /* list of tuples */
   space2_t *retrieve ;         /* list of tuples propobly retrieved. FSUN 09/94 */
   queue1_t *queue_hd ;         /* queue of waiting requests */
   queue1_t *queue_tl ;         /* new requests added at the end */
} tsh ;

queue1_t *tid_q;
int oldsock ;                   /* socket on which requests are accepted */
int newsock ;                   /* new socket identifying a connection */
u_short this_op ;               /* the current operation that is serviced */
char mapid[MAP_LEN];
int EOT = 0;                    /* End of task tuples mark */
int TIDS = 0;
int total_fetched = 0;

/*  Prototypes.  */

void OpPut(/*void*/) ;
void OpGet(/*void*/) ;
void OpExit(/*void*/) ;
void OpRetrieve(/*void*/);
void OpGethostInfo(/*void*/);
void OpTshSot();
void OpTshEot();

int initFromsocket(/*int*/) ;
void initFromline(/*char *, char **/) ;
int initFromline2(/*char *, char **/) ;
int initCommon(/*void*/) ;
void start(/*void*/) ;
void deleteSpace(/*void*/) ;
void deleteQueue(/*void*/) ;
int consumeTuple(/*space1_t **/) ;
int sendTuple(/*queue_t *, space1_t **/) ;
space1_t *findTuple(/*char **/) ;
void deleteTuple(/*space1_t **/) ;
queue1_t *findRequest(/*char **/) ;
void deleteRequest(/*queue_t *q*/) ;
void sigtermHandler(/*void*/) ;
int getTshport(/*void*/) ;
int mapTshport(/*void*/) ;
int unmapTshport(/*void*/) ;
int match(/*char *, char **/) ;
int guardf(/*u_long hostid*/);


/*
 * ==========================================================================
 */


space1_t *createTuple(/*char *, char *, u_long, u_short*/) ;
short int storeTuple(/*space1_t **/) ;

queue1_t *createRequest(tsh_get_it in);
int storeRequest(queue1_t *q);

void deleteTerminator(char *workerid);
void cleanupSpace(char *workerid);
void cleanupQueue(char *workerid);

void advOpActivate();
void advOpCleanupQueue();
void advOpCleanupSpace();
void advOpSendData();
void advOpReadData();
void advOpSetToken();
void advOpGetToken();
void OpRmAll();

int advFindRequest(space1_t *s);
int advFindTuple(queue1_t *q);
int advMatch(char *expr, char *name);
int advHasIt(char *str, char c);
int advScalar(space1_t *o, space1_t *s);
int advIsFull(char *name);
int advDimms(char *expr, char septr);
int advResolveRequest(queue1_t *q);

int FullTupleToPartReq1(space1_t *s, queue1_t *q);
int FullTupleToPartReq2(space1_t *s, queue1_t *q);
int PartTupleToFullReq1(space1_t *s, queue1_t *q);
int PartTupleToFullReq2(space1_t *s, queue1_t *q);
int createTokenTuples1(tsh_put_it in, char *d);
int createTokenTuples2(tsh_put_it in, char *d);

