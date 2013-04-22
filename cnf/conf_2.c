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

   File: conf_2.c

   Developers: Avi Freedman and Hasnain Rangwalla

   Last Change: 6/28/89

*/

#include "conf_2.h"		/* include files, defines and global decl */

/*
   switches to turn on (TRUE) or off (FALSE) debugging statements in the
   respective functions
*/
#define DBG_ASSERT_APPLICATION_NAME FALSE
#define DBG_CLEAR_NODE_INFO FALSE
#define DBG_SAVE_NET_NODES FALSE
#define DBG_BEGIN_SYNONYM_PROC FALSE
#define DBG_ASSERT_SYNONYM_NAME FALSE
#define DBG_BEGIN_EQUIVS_PROC FALSE
#define DBG_BEGIN_NODE_PROC FALSE
#define DBG_END_NODE_PROC FALSE
#define DBG_END_MF_PROC FALSE
#define DBG_ENTER_INTO_MF_LISTS FALSE
#define DBG_FREE_MF_DATA FALSE
#define DBG_FIND_OR_ENTER_NODE FALSE

extern int run_time_debug;  /* 0: no debug; 1: brief debug, 2: more detailed */ 
extern int comp_time_debug;
extern int run_time_log;

struct node *make_a_node();

/*-----------------------------------------------------------------------*/
assert_application_name()
{
   strcpy(application_name, string);
   CONF_DBG2(DBG_ASSERT_APPLICATION_NAME, application name is % s, string);
}

/******************************************************************************/
/*
   net spec functions
*/
/******************************************************************************/

/*-----------------------------------------------------------------------*/
begin_net_node()
/*
   creates a net_spec node and appends it to the end of the linked list
   headed by global ptr net_nodes. global ptr curr_net_nodes is updated to
   point to the new node
*/
{
   CONF_DBG(DBG_CLEAR_NODE_INFO, starting net node);

   if (net_nodes == NULL)	/* list empty? */
   {				/* start list */
      net_nodes = (struct net_spec *)
	  malloc(sizeof(struct net_spec));
      curr_net_node = net_nodes;
      net_nodes->next = NULL;
   }
   else
   {				/* append to list */
      curr_net_node->next = (struct net_spec *)
	 malloc(sizeof(struct net_spec));
      curr_net_node = curr_net_node->next;
      curr_net_node->next = NULL;
   }
}

/*-----------------------------------------------------------------------*/
save_net_node()
{
   CONF_DBG(DBG_SAVE_NET_NODES, processing net nodes);
}

/*-----------------------------------------------------------------------*/
assert_os()
{
   strncpy(curr_net_node->os, string, 128);
}

/*-----------------------------------------------------------------------*/
assert_node_name()
{
   strncpy(curr_net_node->name, string, 128);
}

/*-----------------------------------------------------------------------*/
assert_address()
{
   strncpy(curr_net_node->address, quot_string, 128);
}

/******************************************************************************/
/*
   synonym functions
*/
/******************************************************************************/

/*-----------------------------------------------------------------------*/
begin_synonym_proc()
/*
   creates a list_of_syn_lists node and appends it to the end of the
   linked list headed by global ptr synonyms. Each such node contains the
   head of a linked list of syn_node, which is initialized to an empty
   list. global ptrs curr_syn_list and curr_syn_node are updated to point
   to the newly created node and NULL respectively.
*/
{
   CONF_DBG(DBG_BEGIN_SYNONYM_PROC, beginning synonym processing);

   if (synonyms == NULL)	/* list empty? */
   {				/* start a list */
      synonyms = (struct list_of_syn_lists *)
	 malloc(sizeof(struct list_of_syn_lists));
      curr_syn_list = synonyms;
      curr_syn_list->next = NULL;
      curr_syn_list->curr_list = NULL;
      curr_syn_node = NULL;
   }
   else
   {				/* append to list */
      curr_syn_list->next = (struct list_of_syn_lists *)
	 malloc(sizeof(struct list_of_syn_lists));
      curr_syn_list = curr_syn_list->next;
      curr_syn_list->next = NULL;
      curr_syn_list->curr_list = NULL;
      curr_syn_node = NULL;
   }
}

/*-----------------------------------------------------------------------*/
assert_synonym_name()
/*
   creates a syn_node and appends it to the end of the list headed by the
   curr_list ptr of the list_of_syn_lists node pointed to by global
   curr_syn_list. global ptr curr_syn_node points to the end of the list
   to which the created node must be appended. It copies the synonym name
   into the created node and updates global ptr curr_syn_node.
*/
{
   CONF_DBG2(DBG_ASSERT_SYNONYM_NAME, assert_synonym_name % s, string);

   if (curr_syn_node == NULL)	/* list empty? */
   {				/* start list */
      curr_syn_node = (struct syn_node *)
	 malloc(sizeof(struct syn_node));
      curr_syn_list->curr_list = curr_syn_node;
      curr_syn_node->next = NULL;

      curr_syn_node->name = (char *)	/* make room for name */
	 malloc(sizeof(string));
      strcpy(curr_syn_node->name, string);
   }
   else
   {				/* append to list */
      curr_syn_node->next = (struct syn_node *)
	 malloc(sizeof(struct syn_node));
      curr_syn_node = curr_syn_node->next;
      curr_syn_node->next = NULL;

      curr_syn_node->name = (char *)	/* make room for name */
	 malloc(sizeof(string));
      strcpy(curr_syn_node->name, string);
   }
}

/*-----------------------------------------------------------------------*/
process_synonyms()
{
   adjoin_synonym_lists();
   make_unique_synonym_lists();
   put_synonyms_into_mf_lists();
}

/*-----------------------------------------------------------------------*/
adjoin_synonym_lists()
/*
   the parser along with begin_synonym_proc and assert_synonym_proc builds
   a linked list of synonym list headers which point to lists of synonyms
   as they occur in the CSL source. global ptr synonyms is the header of
   this data structure. This func applies the transitivity property of
   synonyms to group together all lists that have common synonym names.
   Thus if A B C and B E F are two separate syn decl, then this func
   groups these (in the same data struc) as A B C D E F

for every element in the 1st list (the 1st element) every other element of
   all other lists (the 2nd elements) succeeding it are checked for
   duplicates. In case of dupl, the second list is immediately appended to
   the first one and the node containing it's header is removed from the
   list headed by global ptr synonyms. checking is then continued.
   checking ends when less than 2 lists are available
*/
{
   int found;
   struct list_of_syn_lists *lslp1,	/* 1st list header node */
                    *lslp2,	/* 2nd list header node */
                    *lslp_tmp;
   struct syn_node *snp1,	/* 1st element in 1st list */
           *snp2;		/* 2nd element in 2nd list */

   lslp1 = synonyms;
   while (lslp1 != NULL && lslp1->next != NULL)	/* while 2 lists at least */
   {
      find_end(lslp1);		/* find end of 1st in global curr_syn_node */
      snp1 = lslp1->curr_list;
      while (snp1 != NULL)	/* for each 1st element */
      {
	 lslp2 = lslp1;
	 while (lslp2->next != NULL)	/* for each 2nd list */
	 {
	    snp2 = lslp2->next->curr_list;
	    found = FALSE;
	    while (snp2 != NULL)/* for each 2nd element */
	    {
	       if (strcmp(snp1->name, snp2->name) == 0)	/* duplicate? */
	       {
		  curr_syn_node->next = lslp2->next->curr_list;	/* adjoin */
		  /* delete 2nd list from list of syn lists */
		  lslp_tmp = lslp2->next;
		  lslp2->next = lslp2->next->next;
		  free(lslp_tmp);
		  find_end(lslp1);	/* find new end */
		  found = TRUE;
		  break;
	       }
	       snp2 = snp2->next;
	    }
	    if (!found)		/* 2nd list not deleted? */
	       lslp2 = lslp2->next;	/* advance ptr */
	 }
	 snp1 = snp1->next;
      }
      lslp1 = lslp1->next;
   }
}

/*-----------------------------------------------------------------------*/
find_end(lslp)
   struct list_of_syn_lists *lslp;

/*
   find the end of synonym list (whose header is in lslp) and return the
   ptr to last node in curr_syn_node
*/
{
   curr_syn_node = lslp->curr_list;
   while (curr_syn_node->next != NULL) {
      curr_syn_node = curr_syn_node->next;
   }
}

/*-----------------------------------------------------------------------*/
make_unique_synonym_lists()
/*
   removes redundancies in synonym declarations by removing duplicate
   synonym names in each synonym list (in the list of synonym lists headed
   by global ptr synonyms).
*/
{
   struct syn_node *s1,		/* 1st node in syn list */
           *s2,			/* 2nd node in syn list */
           *p_node;		/* node previous to 2nd */

   curr_syn_list = synonyms;
   while (curr_syn_list != NULL)/* for each synonym list */
   {
      s1 = curr_syn_list->curr_list;	/* starting at the head of syn
					   list */
      p_node = NULL;
      while (s1 != NULL)	/* for every syn node */
      {
	 if (s1->next != NULL)	/* if this is not the last */
	 {
	    s2 = s1->next;
	    p_node = s1;
	    while (s2 != NULL)	/* for all succeeding */
	    {
	       if (strcmp(s1->name, s2->name) == 0)	/* if duplicate */
	       {		/* delete the 2nd one and advance ptr */
		  p_node->next = s2->next;
		  free(s2);
		  s2 = p_node->next;
	       }
	       else
	       {		/* only advance ptrs */
		  p_node = s2;
		  s2 = s2->next;
	       }
	    }
	 }
	 s1 = s1->next;
      }
      curr_syn_list = curr_syn_list->next;
   }
}

/*-----------------------------------------------------------------------*/
put_synonyms_into_mf_lists()
/*
   creates linked lists of lk_list nodes with the curr_node ptr pointing
   to M F nodes in the M F lists. All the M F nodes in any of these lists
   are those that have been declared as synonymous. The syn_list ptr of
   every  M F node in these lists are made the header of the lk_list. Thus
   from any M F node, it is possible to access the header of the list of
   all nodes that are synonymous to it. For nodes that have not been
   synonimized with any other, a list of a single lk_list node is created
   and is made to point to the node (every node is at least the synonym of
   itself)
*/
{
   struct lk_list *curr_s_list;	/* header of syn list being created */
   struct lk_list *curr_s_node;	/* last element of syn list */

   curr_syn_list = synonyms;
   while (curr_syn_list != NULL)/* for each synonym list */
   {
      curr_s_list = NULL;
      curr_syn_node = curr_syn_list->curr_list;
      while (curr_syn_node != NULL)	/* for each synonym name of a list */
      {
	 multiplex_node = NULL;
	 /* find that name in M or F lists */
	 find_node_by_name(curr_syn_node->name, m_header);
	 find_node_by_name(curr_syn_node->name, f_header);

	if (multiplex_node == NULL)	/* if not found */
	{
		multiplex_node = make_a_node(curr_syn_node->name);
	}
	       /*
	          append it to the doubly linked list of syn headed by
	          curr_s_list
	       */
	      if (curr_s_list == NULL)	/* if list is empty */
	       {		/* create 1st node */
		  curr_s_list = (struct lk_list *)
		     malloc(sizeof(struct lk_list));
		  memset(curr_s_list, 0x0, sizeof(struct lk_list));
		  curr_s_node = curr_s_list;
		  curr_s_node->curr_node = multiplex_node;
		  curr_s_node->prev = NULL;
		  curr_s_node->next = NULL;
	       }
	       else
	       {		/* simply append */
		  curr_s_node->next = (struct lk_list *)
		     malloc(sizeof(struct lk_list));
		  memset(curr_s_node->next, 0x0, sizeof(struct lk_list));
		  curr_s_node->next->prev = curr_s_node;
		  curr_s_node = curr_s_node->next;
		  curr_s_node->curr_node = multiplex_node;
		  curr_s_node->next = NULL;
	       }

	 curr_syn_node = curr_syn_node->next;
      }

      /* make M F nodes point to curr_s_list */
      curr_s_node = curr_s_list;
      while (curr_s_node != NULL)	/* for every syn in created list */
      {
	 if (curr_s_node->curr_node != NULL)
	 {
	    curr_s_node->curr_node->syn_list = curr_s_list;
	 }
	 curr_s_node = curr_s_node->next;
      }

      curr_syn_list = curr_syn_list->next;
   }
   /*
      for all  F M nodes with empty synonym list make a single node
      lk_list pointing  to itself
   */
   curr_f = f_header;
   while (curr_f != NULL)
   {
      if (curr_f->syn_list == NULL)
      {
	 curr_f->syn_list = (struct lk_list *) malloc(sizeof(struct lk_list));
	 memset(curr_f->syn_list, 0x0, sizeof(struct lk_list));
	 curr_f->syn_list->next = NULL;
	 curr_f->syn_list->prev = NULL;
	 curr_f->syn_list->curr_node = curr_f;
      }
      curr_f = curr_f->next;
   }
   curr_m = m_header;
   while (curr_m != NULL)
   {
      if (curr_m->syn_list == NULL)
      {
	 curr_m->syn_list = (struct lk_list *) malloc(sizeof(struct lk_list));
	 memset(curr_m->syn_list, 0x0, sizeof(struct lk_list));
	 curr_m->syn_list->next = NULL;
	 curr_m->syn_list->prev = NULL;
	 curr_m->syn_list->curr_node = curr_m;
      }
      curr_m = curr_m->next;
   }
}

struct node *make_a_node(name)
char *name; 
{
	struct node *t1;

	t1 = (struct node *)malloc(sizeof(struct node));
	memset(t1, 0x0, sizeof(struct node));
	strcpy(t1->l_name,name);
	t1->type = 's';
	return t1;
}

/*-----------------------------------------------------------------------*/
check_syn_mf_errors()
{
   struct node *np;
   struct lk_list *lklp;

   np = m_header;
   while (np != NULL)
   {
      lklp = np->syn_list;
      while (lklp != NULL)
      {
	 if (lklp->curr_node->type != 'M')
	 {
	    printf("%s: cannot be a synonym with an M type",
		   lklp->curr_node->l_name);
	    exit(-1);
	 }
	 lklp = lklp->next;
      }
      np = np->next;
   }

   np = f_header;
   while (np != NULL)
   {
      lklp = np->syn_list;
      while (lklp != NULL)
      {
	 if (lklp->curr_node->type != 'F')
	 {
	    printf("%s: cannot be a synonym with an F type",
		   lklp->curr_node->l_name);
	    exit(-1);
	 }
	 printf("%s :: a syn with a F type\n",lklp->curr_node->l_name);
	 lklp = lklp->next;
      }
      np = np->next;
   }
}

/******************************************************************************/
/*
   declaration functions
*/
/******************************************************************************/

/*-----------------------------------------------------------------------*/
begin_equivs_proc()
/*
   creates an lk_list node and appends it to the end of the doubly linked
   list headed by temp_head. global ptr curr_lk_list is made to point to
   the new node. global ptr curr_temp_node is set to NULL
*/
{

   CONF_DBG(DBG_BEGIN_EQUIVS_PROC, beginning equivs processing);

   if (temp_head == NULL)	/* is list empty? */
   {				/* start a list */
      temp_head = (struct lk_list *)
	 malloc(sizeof(struct lk_list));
      memset(temp_head, 0x0, sizeof(struct lk_list));
      curr_lk_list = temp_head;
      curr_lk_list->next = NULL;
      curr_lk_list->prev = NULL;
      curr_lk_list->curr_node = NULL;
      curr_temp_node = NULL;
   }
   else				/* simply append */
   {
      curr_lk_list->next = (struct lk_list *)
	 malloc(sizeof(struct lk_list));
      memset(curr_lk_list->next, 0x0, sizeof(struct lk_list));
      curr_lk_list->next->prev = curr_lk_list;
      curr_lk_list = curr_lk_list->next;
      curr_lk_list->next = NULL;
      curr_lk_list->curr_node = NULL;
      curr_temp_node = NULL;
   }
}

/*-----------------------------------------------------------------------*/
begin_node_proc()
/*
   creates an M F type node and appends it to the end of the list whose
   header is in the lk_list node pointed to by global curr_lk_list. global
   curr_temp_node is made to point to new node. Most components of this
   new node are appropriately initialized
*/
{
   struct node *t1;

   CONF_DBG(DBG_BEGIN_NODE_PROC, beginning node processing);

   t1 = (struct node *) malloc(sizeof(struct node));
   memset(t1, 0x0, sizeof(struct node));

   if (curr_lk_list->curr_node == NULL)	/* is list empty? */
   {	/* create first node, and start list */
      curr_lk_list->curr_node = t1; 
      curr_temp_node = t1;
      t1->next = NULL;
   }
   else
   {	/* create node and append */
      t1->next = curr_temp_node;
      copy_attribs(t1,curr_temp_node,0);
      curr_temp_node->prev = t1;
      curr_temp_node = t1;
      curr_lk_list->curr_node = t1;
   }
}

/*-----------------------------------------------------------------------*/
end_node_proc()
/*
   sets the type and status of the M F node pointed to by global
   curr_temp_node to that of the first node in the list after an M: or F:
   (headed by curr_lk_list->curr_node)
*/
{
   CONF_DBG2(DBG_END_NODE_PROC, end node proc % s, curr_temp_node->l_name);

   curr_temp_node->status = curr_lk_list->curr_node->status;
   curr_temp_node->type = curr_lk_list->curr_node->type;
}

/*-----------------------------------------------------------------------*/
end_mf_proc()
{
	struct lk_list *t1;
	struct node *t2;

   CONF_DBG(DBG_END_MF_PROC, ending mf_processing);

   enter_into_mf_lists();

   free_mf_data();
}

/*-----------------------------------------------------------------------*/
enter_into_mf_lists()
/*
   after a complete M F declaration has been parsed (a series of M: F:
   lists), and  all the nodes are collected in M lists and F lists whose
   headers are linked by an lk_list (headed by temp_head), all these nodes
   are entered into the M F lists headed by global m_header and f_header
   respectively.
*/
{
	struct node *nodep,*basenp;

   CONF_DBG(DBG_ENTER_INTO_MF_LISTS, enter_into_mf_lists);

   curr_lk_list = temp_head;
   while (curr_lk_list != NULL)	/* for each M F list */
   {
      curr_temp_node = curr_lk_list->curr_node;
      while (curr_temp_node != NULL)	/* for each node */
      {
	 find_or_enter_node(curr_temp_node);	/* create new only if not
						   already entered */
	 basenp = multiplex_node;
	 nodep = (curr_lk_list->prev == NULL)?   
			NULL : curr_lk_list->prev->curr_node;
	 while (nodep != NULL)
	 {
		find_or_enter_node(nodep);
		enter_pred(basenp,multiplex_node);
	        nodep = nodep->next;
	 }
	 curr_temp_node = curr_temp_node->next;
      }
      curr_lk_list = curr_lk_list->next;
   }
}

/*-----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------*/
enter_pred(basenp,pred)
   struct node *basenp,*pred;

/*
   enters the node pointed to by multiplex node in the pred list of the
   node pointed  to by basenp, only if it is not already entered
*/
{
   struct lk_list *t1; 	/* search ptr for duplicates */
   struct node *t2;

   if (comp_time_debug > 2 ) 
      printf(" enter_pred: pred(%s)<->>based(%s)\n", 
             pred->l_name,basenp->l_name); 

   t1 = pred->succ_list;
   while (t1 != NULL)
   {
	if (strcmp(t1->curr_node->l_name,basenp->l_name) == 0)
		return;
	t1 = t1->next;
   }

   /* not entered so append it at the end */
   t1 = (struct lk_list *) malloc(sizeof(struct lk_list));
   memset(t1, 0x0, sizeof(struct lk_list));
   if (pred->succ_list != NULL)
   pred->succ_list->prev = t1;
   t1->next = pred->succ_list;
   t1->curr_node = basenp;
   t1->prev = NULL;
   pred->succ_list = t1;

   t1 = basenp->pred_list;
   while (t1 != NULL)
   {
	if (strcmp(t1->curr_node->l_name,pred->l_name)==0)
		return;
	t1 = t1->next;
   }
   t1 = (struct lk_list *) malloc(sizeof(struct lk_list));
   memset(t1, 0x0, sizeof(struct lk_list));
   t1->next = basenp->pred_list;
   if (basenp->pred_list != NULL) 
   basenp->pred_list->prev = t1;
   basenp->pred_list = t1;
   t1->curr_node = pred;
   t1->prev = NULL;

   if (comp_time_debug > 2)
   {
 	t1 = pred->succ_list;
	printf(" checking links  ......\n");
	while(t1 != NULL)
	{
	printf(" succ(%s) -> (%s)\n",pred->l_name,t1->curr_node->l_name);
		t1 = t1->next;
	}
	t1 = basenp->pred_list;
	printf(" checking basenp preds ......\n");
	while (t1 != NULL)
	{
	printf(" pred(%s) <- (%s)\n",basenp->l_name,t1->curr_node->l_name);
		t1= t1->next;
	}
	printf(" Printing global data::::::::::\n");
	t2 = m_header;
	while (t2 != NULL) 
	{
		printf(" Module(%s) :\n",t2->l_name);
		t1 = t2->succ_list;
		printf("    succ: ");
		while (t1 != NULL)
		{
			printf(" [%s]",t1->curr_node->l_name);
			t1 = t1->next;
		}
		printf("\n");
		t1 = t2->pred_list;
		printf("    Pred: ");
		while (t1 != NULL)
		{
			printf(" [%s]",t1->curr_node->l_name);
			t1=t1->next;
		}
		printf("\n");
		t2 = t2->next;
	}
   } /* of debug>2 */

}

/*-----------------------------------------------------------------------*/
enter_succ(basenp)
   struct node *basenp;

/*
   enters the node pointed to by multiplex node in the succ list of the
   node pointed  to by basenp, only if it is not already entered
*/
{
   struct lk_list *lklp,	/* search ptr for duplicates */
          *lklp2;		/* ptr previous to lklp */

   if (basenp->succ_list == NULL)	/* is succ list empty? */
   {				/* start list and enter node */
      basenp->succ_list = (struct lk_list *) malloc(sizeof(struct lk_list));
      memset(basenp->succ_list, 0x0, sizeof(struct lk_list));
      basenp->succ_list->next = NULL;
      basenp->succ_list->prev = NULL;
      basenp->succ_list->curr_node = multiplex_node;
      return;
   }

   lklp = basenp->succ_list;
   lklp2 = lklp->prev;
   while (lklp != NULL)		/* check each succ in list */
   {				/* if it has been entered then do nothing */
      if (strcmp(lklp->curr_node->l_name, multiplex_node->l_name) == 0)
	 return;
      lklp2 = lklp;
      lklp = lklp->next;
   }

   /* not entered so append it at the end */
   lklp2->next = (struct lk_list *) malloc(sizeof(struct lk_list));
   memset(lklp2->next, 0x0, sizeof(struct lk_list));
   lklp = lklp2->next;
   lklp->prev = lklp2;
   lklp->next = NULL;
   lklp->curr_node = multiplex_node;

}

/*-----------------------------------------------------------------------*/
free_mf_data()
/*
   after an M F declaration is parsed and all the node information
   collected in the lk_list list headed by temp_head, and the data is
   processed and used, this procedure frees all the memory held in these
   data structures.
*/
{
   struct lk_list *lklp;
   struct node *nodep;

   CONF_DBG(DBG_FREE_MF_DATA, free_mf_data);

   curr_lk_list = temp_head;
   while (curr_lk_list != NULL)	/* for each M F list */
   {
      curr_temp_node = curr_lk_list->curr_node;
      while (curr_temp_node != NULL)	/* for each node in list */
      {
	 nodep = curr_temp_node;
	 curr_temp_node = curr_temp_node->next;
	 free(nodep);		/* free node */
      }
      lklp = curr_lk_list;
      curr_lk_list = curr_lk_list->next;
      free(lklp);		/* free list header */
   }
   temp_head = NULL;		/* init header of entire structure */
}

/*-----------------------------------------------------------------------*/
find_or_enter_node(t_node)
   struct node *t_node;

/*
   checks if the node pointed to by t_node has already been entered in the
   M or F lists (headed by m_header/f_header resp. depending on it's type
   being M or F). If it has been entered, then it returns a ptr to it in
   global multiplex_node, otherwise it creates and appends a new node at
   the end of the M or F list and returns a ptr to it in global
   multiplex_node. The new node if created has data identical to that of
   *t_node. curr_m and curr_f are made to point to the last M and F node
   respectively.
*/
{
   struct node *curr_search,
       *new_node;

 CONF_DBG2(DBG_FIND_OR_ENTER_NODE, find_node called with % s, t_node->l_name);

   /* first, try to find the node in both lists */
   if (t_node->type == 'M' || t_node->type == 'm')	/* is it M type? */
   {
      if (m_header == NULL)	/* is list empty? */
      {				/* start list */

	 new_node = (struct node *) malloc(sizeof(struct node));
	 memset(new_node, 0x0, sizeof(struct node));
/******
	 memcpy(new_node, t_node, sizeof(struct node));
*******/
	 copy_attribs(new_node,t_node,1); 
	 new_node->node_num = 1;	/* 1st node */
	 multiplex_node = new_node;
	 curr_m = new_node;
	 m_header = new_node;
	 return (1);
      }

      if (find_node_by_name(t_node->l_name, m_header) == 1)	
	 return (1);		/* simply return */

      /* not found, create and append */
      new_node = (struct node *) malloc(sizeof(struct node));
      memset(new_node, 0x0, sizeof(struct node));
      copy_attribs(new_node,t_node,1);
      curr_m->next = new_node;
      new_node->prev = curr_m;
      new_node->node_num = curr_m->node_num + 1;
      curr_m = curr_m->next;
      curr_m->next = NULL;
      multiplex_node = curr_m;
      return (1);
   }

   if (t_node->type == 'F' || t_node->type == 'f')	/* is it F type? */
   {
      if (f_header == NULL)	/* is list empty? */
      {				/* start list */
	 new_node = (struct node *) malloc(sizeof(struct node));
         memset(new_node, 0x0, sizeof(struct node));
	 copy_attribs(new_node,t_node,1);
	 f_header = new_node;
	 curr_f = f_header;
	 curr_f->next = NULL;
	 curr_f->prev = NULL;
	 curr_f->node_num = 1;	/* 1st node */
	 multiplex_node = curr_f;
	 return (1);
      }

      if (find_node_by_name(t_node->l_name, f_header) == 1)	/* found? */
	 return (1);		/* simply return */

      /* not found, create and append */
      new_node = (struct node *) malloc(sizeof(struct node));
      memset(new_node, 0x0, sizeof(struct node));
      copy_attribs(new_node,t_node,1);
      curr_f->next = new_node;
      new_node->prev = curr_f;
      new_node->node_num = curr_f->node_num + 1;
      curr_f = curr_f->next;
      curr_f->next = NULL;
      multiplex_node = curr_f;
      return (1);
   }
}

copy_attribs(new,t,sw)
struct node *new,*t;
int sw;
{
	strcpy(new->l_name,t->l_name);
	if (sw) {
	strcpy(new->P_name,t->P_name);
	strcpy(new->loc,t->loc);
	strcpy(new->e_P_name,t->e_P_name);
	strcpy(new->e_loc,t->e_loc);
	strcpy(new->path,t->path);
	strcpy(new->e_path,t->e_path); 
	}
	new->type = t->type;
	new->status = t->status;
	new->node_num = t->node_num;
	new->org = t->org;
	new->f = t->f;
	new->d = t->d;
	new->t = t->t;
	strcpy(new->mode,t->mode);
	new->p_list = t->p_list;
/*****
	new->prev = t->prev;
	new->succ_list = t->succ_list;
	new->pred_list = t->pred_list;
	new->next = t->next;
********/
}

/*-----------------------------------------------------------------------*/
find_node_by_name(name, node_list)
   char *name;
   struct node *node_list;

/*
   searches for a node with l_name == *name in the list of nodes whose 1st
   node is pointed to by node_list. returns 1 if found and -1 otherwise
*/
{
   struct node *foo;

   foo = node_list;
   while (foo != NULL)		/* for every node starting at given ptr */
   {
      if (strcmp(foo->l_name, name) == 0)	/* does it have same
						   logical name? */
      {
	 multiplex_node = foo;	/* return a ptr to it */
	 return (1);
      }

      foo = foo->next;
   }

   return (-1);			/* not found */
}

/*-----------------------------------------------------------------------*/
assert_m()			/* M/F */
{
   curr_temp_node->type = 'M'; 
}

assert_f()
{
   curr_temp_node->type = 'F';
}

/*-----------------------------------------------------------------------*/
assert_status(c)		/* + or blank */
   char c;
{
   curr_temp_node->status = c;
}

/*-----------------------------------------------------------------------*/
assert_l_name()
{
   strncpy(curr_temp_node->l_name, string, 10);
}

/*-----------------------------------------------------------------------*/
assert_l_name_prefix()
{
   strncpy(curr_temp_node->l_name_prefix, string, 10);
}

/*-----------------------------------------------------------------------*/
assert_l_name_suffix()
{
   strncpy(curr_temp_node->l_name_suffix, string, 10);
}

/*-----------------------------------------------------------------------*/
assert_P_name()
{
   strcpy(curr_temp_node->P_name, string);
}

/*-----------------------------------------------------------------------*/
assert_path()
{
   strcpy(curr_temp_node->path, string);
}

/*-----------------------------------------------------------------------*/
assert_e_path() 
{
   strcpy(curr_temp_node->e_path, string);
}

/*-----------------------------------------------------------------------*/
assert_e_P_name()
{
   strcpy(curr_temp_node->e_P_name, string);
}

/*-----------------------------------------------------------------------*/
assert_e_loc()
{
   strcpy(curr_temp_node->e_loc, string);
}


assert_loc()
{
	strcpy(curr_temp_node->loc,string);
}

/*-----------------------------------------------------------------------*/
assert_org()
{
   int i;

   for (i = 0; i < 5; i++)	/* convert to upper case */
      if (islower(string[i]))
	 string[i] = toupper(string[i]);
   if (strcmp(string,"PIPE")==0) curr_temp_node->org = 'm';
   if (strcmp(string,"TS")==0)   curr_temp_node->org = 't';
   if (strcmp(string,"MDL")==0)  curr_temp_node->org = 'd';
   if (strcmp(string,"DEV")==0)  curr_temp_node->org = 'v';
   if (strcmp(string,"GRP")==0)  curr_temp_node->org = 'g';
   if (strcmp(string,"SLAVE")==0) curr_temp_node->org = 'l';
   if (strcmp(string,"SEQ")==0)  curr_temp_node->org = 's';
}
/*-----------------------------------------------------------------------*/
assert_fac()
{
   curr_temp_node->f = atoi(integer);
}
/*-----------------------------------------------------------------------*/
assert_t()
{
   curr_temp_node->t = atoi(integer);
}
/*-----------------------------------------------------------------------*/
assert_d()
{
   curr_temp_node->d = atoi(integer);
}
/*-----------------------------------------------------------------------*/
assert_mode()
{
   strcpy(curr_temp_node->mode,string);
}
/*-----------------------------------------------------------------------*/
assert_parm()
{
	struct parm_list *pptr;
	pptr =(struct parm_list *) malloc(sizeof(struct parm_list));
        memset(pptr, 0x0, sizeof(struct parm_list));
	pptr->parm = (char *) malloc(strlen(string)+1);
	strcpy(pptr->parm,string);
	pptr->next = curr_temp_node->p_list;
	if (comp_time_debug > 2) printf("received a parm: %s\n",pptr->parm); 
	curr_temp_node->p_list = pptr;
}

assert_parmi()
{
	struct parm_list *pptr;
	pptr =(struct parm_list *) malloc(sizeof(struct parm_list));
        memset(pptr, 0x0, sizeof(struct parm_list));
	pptr->parm = (char *) malloc(strlen(integer)+1);
	strcpy(pptr->parm,integer);
	pptr->next = curr_temp_node->p_list;
	if (comp_time_debug > 2) printf("received a parm: %s\n",pptr->parm); 
	curr_temp_node->p_list = pptr;
}

/*-----------------------------------------------------------------------*/
assert_rec_size()
{
/*
   curr_temp_node->rec_size = atoi(integer);
*/
}
/*-----------------------------------------------------------------------*/
assert_comp_debug()
{
   comp_time_debug= atoi(integer);
}
/*-----------------------------------------------------------------------*/
assert_runtime_debug()
{
   run_time_debug= atoi(integer);
}
assert_runtime_log()
{
   run_time_log = atoi(integer);
}

/*
   error checking functions
*/

/*-----------------------------------------------------------------------*/
check_for_errors()
/*
   after parsing is successfully completed, this function should be called
   to check the semantic validity of the CSL source which the parser does
   not check for. It goes thru the entire list of M and F nodes headed by
   global m_header and f_header and performs some checks.
*/
{
   curr_m = m_header;
   while (curr_m != NULL)	/* for each M node */
   {
     if ( m_syn_err() ) return CSL_ERR;
     if ( m_pr_su_prefix_err() ) return CSL_ERR;
      curr_m = curr_m->next;
   }
   curr_f = f_header;		/* for each F node */
   while (curr_f != NULL)
   {
      if ( f_syn_err() ) return CSL_ERR;
      if ( f_pr_su_number_err() ) return CSL_ERR;
      curr_f = curr_f->next;
   }
}

/*-----------------------------------------------------------------------*/
m_syn_err()
/*
   flags an error if an M type node pointed to by curr_m has been
   synonymized with any other
*/
{
   if (curr_m->syn_list->next != NULL)	/* is the list > 2? */
   {
/*
      printf("\nconf: can't synonymize M types");
	printf("\n	module name(%s)\n",curr_m->l_name);
*/
	curr_m->syn_list = NULL;
	return (CSL_ERR);
/*      exit(CSL_ERR); */
   }
   return 0;
}

/*-----------------------------------------------------------------------*/
m_pr_su_prefix_err()
/*
   flags an error if any pred/succ node of an M node pointed to by curr_m
   has a prefix other than the M node name
*/
{
   struct lk_list *lklp;

   if (curr_m->l_name_prefix[0] != '\0')	/* is the M node name
						   single part? */
   {
      printf("\nconf: M names must be single part");
      return(CSL_ERR);
   }
   lklp = curr_m->pred_list;
   while (lklp != NULL)		/* for all it's predecessors */
   {
      /* if 2 part name and prefix not the same */
      if ((lklp->curr_node->l_name_prefix[0] != '\0') &&
	  (strcmp(lklp->curr_node->l_name_prefix, curr_m->l_name) != 0))
      {
	 printf("\nconf: F name prefix must be the logical name of unique succ");
	 return(CSL_ERR);
      }
      lklp = lklp->next;
   }
   lklp = curr_m->succ_list;
   while (lklp != NULL)		/* for all it's sucessors */
   {
      /* if 2 part name and prefix not the same */
      if ((lklp->curr_node->l_name_prefix[0] != '\0') &&
	  (strcmp(lklp->curr_node->l_name_prefix, curr_m->l_name) != 0))
      {
	 printf("\nconf: F name prefix must be the logical name of unique pred");
	 return(CSL_ERR);
      }
      lklp = lklp->next;
   } 
   return 0;
}

/*-----------------------------------------------------------------------*/
f_syn_err()
/*
   not implemented as yet

checks if the synonyms of the F node pointed to by curr_f are
   synonymizable with the F node. There should be no inconsistencies
   detected. Also it updates the data in each node (if unspecified) to be
   consistently the same as that in all it's other synonyms
*/
{
	return 0;
}

/*-----------------------------------------------------------------------*/
f_pr_su_number_err()
/*
   checks if the datastore (F node) pointed to by curr_f obeys the rules
   of the number of predecessors and successors it can have for it's
   stated organization
*/
{
   /* if MAIL type then it can have at most one successor */
   if ((curr_f->org == 'm' ) &&
       (curr_f->succ_list != NULL) && (curr_f->succ_list->next != NULL))
   {
/****
      printf("\nconf: MAIL type DS can be read by only one module");
      exit(CSL_ERR);
*****/
   }
	return 0;
}

/*************************************************************************
   display functions
*************************************************************************/

/*-----------------------------------------------------------------------*/
show_parsed_data()
/*
   after the CSL source is parsed, this func is invoked to display the
   contents of the data structures used to store all necessary information
   while parsing
*/
{
   print_net_nodes();
   printf("\n\nM nodes:");
   curr_m = m_header;
   while (curr_m != NULL)	/* for all M type nodes */
   {
      print_node(curr_m);
      curr_m = curr_m->next;
   }
   printf("\n\nF nodes:");
   curr_f = f_header;
   while (curr_f != NULL)	/* for all F type nodes */
   {
      print_node(curr_f);
      curr_f = curr_f->next;
   }
}

/*-----------------------------------------------------------------------*/
print_net_nodes()
/*
   prints all the network specification info that was found in N:
   declarations and saved as a linked list in net_spec nodes
*/
{
   int i;

   i = 0;
   curr_net_node = net_nodes;
   printf("\n\nnetwork specifications:\n");
   while (curr_net_node != NULL)/* for each net node */
   {
      i++;
      printf("\nnet node %d: ", i);
      printf("\nname: %s\taddress: %s\tos: %s\t",
	     curr_net_node->name,
	     curr_net_node->address,
	     curr_net_node->os);
      curr_net_node = curr_net_node->next;
   }
}

/*-----------------------------------------------------------------------*/
print_node(nodep)
   struct node *nodep;

/*
   prints the complete contents of the M or F type node pointed to by
   nodep
*/
{
   printf("\n =============================");
   printf("\nnode node_num: %d", nodep->node_num);
   printf("\nnode type:     %c", nodep->type);
   printf("\nnode status:   %c", nodep->status);
   printf("\nnode l_name:   %s", nodep->l_name);
   printf("\nnode l_name_p: %s", nodep->l_name_prefix);
   printf("\nnode l_name_s: %s", nodep->l_name_suffix);
   printf("\nnode loc:      %s", nodep->loc);
   printf("\nnode e_loc:    %s", nodep->e_loc);
   printf("\nnode e_P_name: %s", nodep->e_P_name);
   printf("\nnode P_name:   %s", nodep->P_name);
   printf("\norg:           %c", nodep->org);
   printf("\npred *************\n:");
   show_lk_list(nodep->pred_list);
   printf("\nsucc *************\n:");
   show_lk_list(nodep->succ_list);
   printf("\nsyn ++++++++++++++\n:");
   show_lk_list(nodep->syn_list);
}

/*-----------------------------------------------------------------------*/
show_lk_list(list)
   struct lk_list *list;

/*
   prints the logical names of all the M F nodes that are pointed to by
   the curr_node ptr of lk_list nodes in the linked list of lk_list nodes
   headed by list
*/
{
   struct lk_list *ptr;

   ptr = list;
   if (ptr = NULL) printf(" (nil list)\n");
   while (ptr != NULL)		/* for each lk_list node */
   {
      printf(" ++++ l_name(%s)\n", ptr->curr_node->l_name);
      printf(" ++++ P_name(%s)\n", ptr->curr_node->P_name);
      ptr = ptr->next;
   }
}
