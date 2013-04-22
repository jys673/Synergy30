
/* ERROR CODES from passive object operations */
#define INIT_ER			-100
#define FOPEN_ER		-101
#define OPEN_ER			-102
#define CLOSE_ER		-103
#define READ_ER			-104
#define WRITE_ER		-105
#define TSPUT_ER		-106
#define TSGET_ER		-107
#define TSREAD_ER		-108
#define FREAD_ER		-109
#define FWRITE_ER		-110
#define FPUTS_ER		-111
#define FGETS_ER		-112
#define FPUTC_ER		-113
#define FGETC_ER		-114
#define FSEEK_ER		-115
#define FFLUSH_ER		-116

/* Error messages for object operations */
static char *proc_err_msg[17] = {"Initialization Error", "Object open error",
	"File open error. Check path.", "Object close error",
	"Pipe read error.", "Pipe write error.", "Tuple space put error.",
	"Tuple space get error", "Tuple space read error", "File read error.",
	"File write error.","File puts error.","File gets error.",
	"File putc error.", "File getc error.","File seek error.",
	"File flush error."};

/* DAC Data structure Used in IOLIB */

typedef struct map{
   char type ;   		/* pipe/file/space */
   char dirn ;	        	/* in/out/nil */
   char ref_name[NAME_LEN] ;	/* reference name */
   char obj_name[NAME_LEN] ;	/* object name */
   char phys_name[PATH_LEN];	/* path and physical name. Only for FILE */
   char cpu[PATH_LEN];		/* for PIPE-out, TS and FILE 	*/
   u_long host ;		/* ... 				*/	
   char login[PATH_LEN] ;	/* for remote sngd queries YS94 */ 
   int  sd;			/* for PIPE-in. Setup in cnf_open */
   int  port;			/* for PIPE-in,PIPE-out,TS and FILE */
   int  ret_port;		/* for TS and FILE */
   char open;			/* open status */   
   struct map *next;		/* next pointer */
} sng_map;

struct {
   char csl_name[APP_LEN];	/* Application system name */
   char name[NAME_LEN] ;	/* Component name */
   char appid[NAME_LEN] ;	/* Application id (pid) */
   char protocol[NAME_LEN];	/* TCP,IPX, etc. */
   char args[ARG_LEN][ARGS_MAX];	/* arguments */
   int f ;
   int p ;
   int t ;
   int d ;
   u_short link_cnt ;
   sng_map *link_hd;
   u_long host ;		/* host infor */ 
   int cidport;		/* local cid port */
} sng_map_hd; /* dac_data_ot ; */ 


typedef struct tid_lt {
   char tid[TUPLENAME_LEN];
   char appid[TUPLENAME_LEN];
   u_long host;
   u_short cid_port;
   struct tid_lt *next;
} tid_list;

/* IOLIB Specific Data Structures */

char local_node[PATH_LEN], local_addr[PATH_LEN];
int  init_sw;
int  sng_idx;
sng_map **handles;
host_t *list_host, *end_host;
int  hlist_cnt;
int sys_TIDS;

tid_list *tid_header;

int getRealTuple ();
