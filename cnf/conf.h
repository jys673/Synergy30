/*
   physical node declarations
*/
#define AIX     1
#define MAIL_F 'm'
#define POST_F 'p'
#define SEQ_F  's' 
#define TPS    't'

#define GRP    'g'
#define MDL    'd'
#define DEV    'v'
#define SLAVE  'l'	/* parallel workers */

/*
   synonym declarations
*/

struct syn_node			/* node of a linked list of comma
				   delimited names in a synonym S:
				   declaration */
{
   char *name;
   struct syn_node *next;
};

struct list_of_syn_lists	/* node of a linked list of S:
				   declarations found in the entire
				   configuration program */
{
   struct syn_node *curr_list;	/* header of an S: type decl list */
   struct list_of_syn_lists *next;
};

/*
   node declarations
*/

struct node			/* M or F type graph node found in the CSL
				   input, also node of a doubly linked
				   list */
{
   int node_num;		/* unique positive integer assigned to
				   node */
   char type;			/* M or F */
   char status;			/* + or blank */

   char l_name[128];		/* logical name */
   char l_name_prefix[10];	/* first part of logical name if in two
				   parts */
   char l_name_suffix[10];	/* second part for 2 part names, or first */
   char local_name[128]; 	/* local file name= l_name | m.fx in syn_list*/ 

   char P_name[128];		/* file name */
   char path[128];		/* storage path name */ 
   char loc[128];		/* storage location (cpu id) */ 
   char e_P_name[128];		/* execution name */
   char e_path[128];   		/* execution path name */ 
   char e_loc[128];		/* node of exec location */
   char org;			/* organization for datastores */
				/* For modules:
				   g: GRP
				   d: MDL
				   v: DEV */
			 	/* For files: */
				/* m: MAIL
				   p: POST
				   t: TUPLE SPACE
				   s: STREAM (remote pipe)
				   b: SEQUENTIAL BUFFER (file) */
   /*
      the following 3 are linked lists of nodes with curr_node pointing to
      the node
   */
   struct lk_list *succ_list;	/* successor nodes in directed graph */
   struct lk_list *pred_list;	/* predecessor nodes in directed graph */
   struct lk_list *syn_list;	/* synoym nodes to the current one */
   int  temp;			/* working var */
   int  ts_state;		/* for generating tsd's uniquely */
   int	succ_num;		/* number of predecessors */ 
   int  pred_num;		/* number of successors */ 
   int  syn_num;		/* number of synonyms */
   int f;			/* factoring value (0-100) */
   int p;			/* number of processors */
   int t; 			/* threshold value */
   int d;			/* debug value: 0-5 */
   char mode[2];		/* r,w,a,r+,w+,a+ only for SEQ files */
   struct parm_list *p_list;    /* parameter list */
   struct node *next;		/* ptrs for double linked lists of M F
				   type */
   struct node *prev;
};

struct lk_list			/* node of a doubly linked list of doubly
				   linked lists of M or F type CSL nodes */
{
   struct node *curr_node;	/* header of a doubly linked list of nodes */
   struct lk_list *next;	/* ptrs for doubly linked list of lk_list */
   struct lk_list *prev;
};

struct parm_list
{
   char *parm;
   struct parm_list *next;
};

struct parm_list *parm_header;

struct net_spec {
	char	name[128];
	char	address[128];
	char	os[128];
	char	language[128];
	char  	prot[128];
	char	conf_dir[128];
	struct net_spec	*next;
	} ;


struct net_node
       {
         char *name;
         struct  net_node *next;
         struct  net_node *syn_prot;
       };
