/*---------------------------------------------------------------------
    cnf_sys.h - header file for all Configurator programs and libraries.
    (not intended for use in application programs).

    Author: Doug Bagley
 ---------------------------------------------------------------------*/

/* constants */

#ifndef FALSE
#define  FALSE          0
#endif
#ifndef TRUE
#define  TRUE           1
#endif
#define LATHOS         -1

#define  MAX_PROC       200     /* max number of subprocesses */ 
#define  MAXMSGSIZ	8192	/* max size of message */
#define  MAXNODES	5000	/* maximum number of networked computers */
#define  MAX_FILES	50	/* max # of files for an application program,
				   (for declaring cnf_map[]) */
#define  MAX_STR_LEN    512     /* max chars per string */ 
#define  MBX_ID_LEN	15	/* max len of mailbox name */
#define  TASKNAM_LEN    12	/* max len of a declared network task */
#define  PRCNAM_LEN	12	/* max len of a process name */
#define  CONFIG_ID_LEN  12	/* max len of config name (network obj name)*/
#define  MOD_ID_LEN	128	/* max len of module identifier (also used 
				    as process name) */
#define  DPC_MSG_LEN    80	/* max len of msg from dpc to dpcserv */
#define  STATMSG_LEN    84      /* max len of msg from dpcserv to dpc */
#define  FILE_ID_LEN	64	/* max len of 'filename.ext' */
#define  PHYS_ID_LEN	128	/* max len of complete physical pathname */
#define  NODE_ID_LEN	128	/* max len of node spec */
#define  TASK_ID_LEN	40	/* max len of network task spec */
#define  LINE_LEN	256	/* max len of text file line */
#define  CMD_LEN	256	/* max len of command line to rciserver */
#define  TIME_LEN	22	/* max len of a VMS system time */
#define  RCINAME	"RCISERV"  /* network name of rciserver */
#define  DPCSERVNAM	"DPCSERV"  /* network name of dpcserver */
#define  INIT_SUFFIX	".ini"	/* suffix for config init files */
#define  PRCD_SUFFIX	".prcd"    /* suffix for precedence file */
#define  JCL_SUFFIX	".com"	   /* suffix for module startup file */
#define  DECNET_INFIX   "::\"0="   /* infix for decnet addr: nd::0="name" */
#define  DECNET_SUFFIX  "\""       /* suffix for decnet: " */
#define  ORG_TYPES	"mpsrit"/* mail, post, sequential, random */
#define  MAIL		'm'	/* mail file organization */
#define  POST		'p'	/* post file organization */
#define  SEQ		's'	/* sequential file organization */
#define  TS		't'	/* tuple space */
#define  STREAM 	'i'	/* remote stream (pipe) */
#define  RAND		'r'	/* random file organization */
#define  INPUT_FILE	'<'	/* file is for input */
#define  OUTPUT_FILE	'>'	/* file is for outpout */

/* status values */

#define  CNF_NORMAL  0

#if VMS 
/* for string descriptors */
typedef struct dsc$descriptor STRING_D;

/* for I/O status blocks */
typedef struct {
    short stat, bytes;
    int pid;
} IOSB_S;
#endif

/* macros */

#define  ERRMSG(msg)  fprintf(stderr, msg)

#define  DEBUG2(fun, msg)  \
    if (cnf_debug) { cnf_debugmsg(fun); \
        fprintf(stderr, msg); fprintf(stderr, ".\n"); }
#define  DEBUG3(fun, msg, arg1)  \
    if (cnf_debug) { cnf_debugmsg(fun); \
        fprintf(stderr, msg, arg1); fprintf(stderr, ".\n"); }
#define  DEBUG4(fun, msg, arg1, arg2)  \
    if (cnf_debug) { cnf_debugmsg(fun); \
        fprintf(stderr, msg, arg1, arg2); fprintf(stderr, ".\n"); }
#define  DEBUG5(fun, msg, arg1, arg2, arg3)  \
    if (cnf_debug) { cnf_debugmsg(fun); \
        fprintf(stderr, msg, arg1, arg2, arg3); fprintf(stderr, ".\n"); }
#define  ERROR4(lev, code, fun, msg) \
      { cnf_error(lev, code, fun);  fprintf(stderr, msg); \
        fprintf(stderr, ".\n");  if (lev == 'e') exit(cnf_stat); }
#define  ERROR5(lev, code, fun, msg, arg1) \
      { cnf_error(lev, code, fun);  fprintf(stderr, msg, arg1); \
        fprintf(stderr, ".\n");  if (lev == 'e') exit(cnf_stat); }
#define  ERROR6(lev, code, fun, msg, arg1, arg2) \
      { cnf_error(lev, code, fun);  fprintf(stderr, msg, arg1, arg2); \
        fprintf(stderr, ".\n");  if (lev == 'e') exit(cnf_stat); }
#define  ERROR7(lev, code, fun, msg, arg1, arg2, arg3) \
      { cnf_error(lev, code, fun);  fprintf(stderr, msg, arg1, arg2, arg3); \
        fprintf(stderr, ".\n");  if (lev == 'e') exit(cnf_stat); }

