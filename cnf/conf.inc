
/*-----------------------------------------------------------------------

		Recover Pred count from generate_ctls() [node.temp]

-------------------------------------------------------------------------*/
recover_prcd()
{
	struct node *t1;

	t1 = f_header;
	while (t1 != NULL) 
	{
		t1->temp = t1->pred_num;
		t1 = t1->next;
	}
}

/*-----------------------------------------------------------------------
    Calculate physical names by a) using defaults (dnode,pwd)
			        b) using logical file numbers (for mail)
				c) automatic processor assignment YS94
--------------------------------------------------------------------------*/
calc_phys_names()
{
struct node *t1,*t2;
struct lk_list *tl1,*tl2;
int count, cnt2, node_num;		/* for calc mail file numbers */
char buf[MAX_STR_LEN], last_cpu[MAX_STR_LEN],last_path[MAX_STR_LEN];
int first = 1, idx, Factor, P;
char *retbuf;

t1 = f_header;
node_num = 0;
global_slave_ptr = NULL;
while (t1 != NULL)
{
	t1->node_num = node_num++;
	if (t1->e_loc[0] != 0) /* check legality of addr */
	{
		count = find_host_idx(t1->e_loc);
		if (count < 0)
		{
			printf("++ CONF. (%s) not available.\n",t1->e_loc);
			t1->e_loc[0] = 0;
		}
	}
	if (t1->org == TPS) /* calculate number of workers */
	{	tl1 = t1->succ_list;
		cnt2 = 0; 
		while (tl1 != NULL) 
		{
			if (cnt2 == 0)
			{	strcpy(buf,tl1->curr_node->P_name);
				cnt2 = 1;
			} else {
				if (strcmp(tl1->curr_node->P_name,buf)==0) 
				{
					cnt2++;
				}
			}					
			if (tl1->curr_node->e_loc[0] == 0)
			{
			  retbuf = get_a_slave_cpu(t1->pred_list);
			  if (retbuf != NULL)  strcpy(tl1->curr_node->e_loc, retbuf);
			  if (tl1->curr_node->e_loc[0] == 0)
				strcpy(tl1->curr_node->e_loc,get_a_cpu(1, 0, 0));
			  if (tl1->curr_node->e_path[0] == 0)
					strcpy(tl1->curr_node->e_path,"*");
			  if (isdigit(tl1->curr_node->e_loc[0]))
			  {
				strcpy(buf, ary_node_cap[find_host_idx(
					tl1->curr_node->e_loc)].name);
			  }
			  else strcpy(buf,tl1->curr_node->e_loc);
			  printf("++\tAutomatic program assignment: (%s)->(%s)\n",
				tl1->curr_node->l_name, buf);
			} 
			tl1 = tl1->next;
		}
		first = 1;
		tl1 = t1->pred_list;
		while (tl1 != NULL)
		{
			if (tl1->curr_node->e_loc[0] == 0)
			{
			  retbuf = get_a_slave_cpu(t1->succ_list);
			  if (retbuf != NULL) strcpy(tl1->curr_node->e_loc, retbuf);
			  if (tl1->curr_node->e_loc[0] == 0)
				strcpy(tl1->curr_node->e_loc,get_a_cpu(1, 0, 0));
			  if (isdigit(tl1->curr_node->e_loc[0]))
			  {
				strcpy(buf, ary_node_cap[find_host_idx(
					tl1->curr_node->e_loc)].name);
			  }
			  else strcpy(buf,tl1->curr_node->e_loc);
			  printf("++\tAutomatic program assignment: (%s)->(%s)\n",
				tl1->curr_node->l_name, buf);
			  if (tl1->curr_node->e_path[0] == 0)
			  strcpy(tl1->curr_node->e_path,"*");
			}
			tl1->curr_node->p = cnt2; 
			/* all masters assigned */
			tl1 = tl1->next;
		}
	}
	t1 = t1->next;
}
node_num = 0;
P = getp();
Factor = getf();
// printf("Calc_Phys. getp=(%d) getf(%d)\n", P, Factor);

t1 = m_header;
while (t1 != NULL)
{
	t1->node_num = node_num++;
	if (t1->P_name[0] == '\0') strcpy(t1->P_name,t1->l_name);  
	if (t1->path[0] == '\0') strcpy(t1->path,pwd); 
	if (t1->loc[0] == '\0') strcpy(t1->loc,dnode); 
	if (t1->e_P_name[0] == '\0') strcpy(t1->e_P_name,t1->P_name); 
	if (t1->e_path[0] == '\0') strcpy(t1->e_path, "*"); 
	if (!strcmp(t1->e_path,"/")) strcpy(t1->e_path,"*");	
	if (t1->e_loc[0] != 0)
	{
		count = find_host_idx(t1->e_loc);
		if (count < 0)
		{
			printf("++ CONF. Host (%s) not available.\n",
				t1->e_loc);
			t1->e_loc[0] = 0;
		}
	}
	if (t1->e_loc[0] == '\0')
	{
		strcpy(t1->e_loc,get_a_cpu(1, 0, 0)); 
		if (isdigit(t1->e_loc[0]))
		{ 
		   strcpy(buf, ary_node_cap[find_host_idx(t1->e_loc)].name);
		}
		else strcpy(buf,t1->e_loc);
		printf("++\tAutomatic program assignment: (%s)->(%s)\n",
		t1->l_name, buf);
		if (t1->e_path[0] == 0) strcpy(t1->e_path,"*");
	}
	if (t1->d == 0) t1->d = run_time_debug;
	if (t1->f == 0) t1->f = Factor;
	if (t1->t == 0) t1->t = 1;
	if (t1->p == 0) t1->p = P;
	count = 0;
	cnt2 = 0;
	tl1 = t1->pred_list; 	/* calc mail file numbers */
	while (tl1 != NULL)
	{
	strcpy(buf,find_local_name(tl1->curr_node->syn_list,t1));
	if (strlen(buf) == 0)
	strcpy(buf,find_a_local_name(tl1->curr_node->syn_list,t1));
	strcpy(tl1->curr_node->local_name,buf);
	if (tl1->curr_node->org==MAIL_F)
	{
		sprintf(buf,"%d",count++); 
		strcat(tl1->curr_node->P_name,t1->l_name);
		strcat(tl1->curr_node->P_name,buf);
	}
	cnt2++;
	tl1 = tl1->next;
	}
	t1->pred_num = cnt2; 
	t1->temp = cnt2; 	/* temp = pred_num */
	cnt2 = 0;
	tl1 = t1->succ_list;
	while (tl1 != NULL) 
	{
		tl1 = tl1->next;
		cnt2++;
	}
	t1->succ_num = cnt2; 
	t1->ts_state = 0; 
	t1 = t1->next;
}
t1 = f_header;
while (t1 != NULL)
{	
	t1->ts_state = 0;
	cnt2 = 0;
	if (t1->loc[0] == '\0') strcpy(t1->loc,dnode);
	tl1 = t1->pred_list;
	while (tl1 != NULL) 
	{
		tl1 = tl1->next;
		cnt2++;
	}
	t1->pred_num = cnt2; 
	t1->temp = cnt2; 		/* temp = pred_num */
	/* temp will decrease as N->1 connection implements */
	cnt2 = 0;
	tl1 = t1->succ_list;
	while (tl1 != NULL) 
	{
		tl1 = tl1->next;
		cnt2++;
	}
	t1->succ_num = cnt2; 
	switch(t1->org) {
	case MAIL_F : 	if (t1->P_name[0] == '\0')
			strcpy(t1->P_name,find_target_mail(t1)); 
			strcpy(t1->e_loc,find_target_e_loc(t1));
	       	break;
	case TPS    : 	if (t1->P_name[0] == '\0')
			sprintf(t1->P_name,"%s%d$%s",dnode,getpid(),
				t1->l_name);
			if (t1->e_loc[0] == 0)
			{
				if (t1->pred_num < t1->succ_num)
				strcpy(t1->e_loc,find_host(t1->pred_list));
				else
				strcpy(t1->e_loc,find_host(t1->succ_list));
				if (isdigit(t1->e_loc[0]))
				{ 
		   			strcpy(buf, ary_node_cap[
						find_host_idx(t1->e_loc)].name);
				}
				else strcpy(buf,t1->e_loc);
				printf("++\tAutomatic object assignment: (%s)->(%s) pred(%d) succ(%d) \n",
				t1->l_name,buf, t1->pred_num, t1->succ_num);
			}
			break; 
	default     : 	if (t1->P_name[0] == '\0')
			strcpy(t1->P_name,t1->l_name);
		      	if (t1->org == '\0')
		 	t1->org = SEQ_F;
			if (t1->path[0] == '\0') strcpy(t1->path,pwd); 
			if (t1->loc[0] == '\0') strcpy(t1->loc,dnode); 
			/* file location must be specified manually */
			if (t1->e_loc[0] == 0) {
				strcpy(t1->e_loc, dnode);
				if (isdigit(t1->e_loc[0]))
				{ 
		   			strcpy(buf, ary_node_cap[
						find_host_idx(t1->e_loc)].name);
				}
				else strcpy(buf,t1->e_loc);
				printf("++\tAutomatic object assignment: (%s)->(%s)\n",
				t1->l_name,buf);
			}
			if (t1->e_P_name[0] == '\0') 
				strcpy(t1->e_P_name,t1->P_name); 
			if (t1->e_path[0] == '\0') 
				strcpy(t1->e_path,pwd);  
			break;
	}
	t1 = t1->next;
}
} /* of calc_phys_name */

int getf()
{
	tnd = m_header;
	while (tnd != NULL)
	{
		if (tnd->f != 0) return(tnd->f);
		tnd=tnd->next;
	}
	return 0;
}

int getp()
{
	tnd = m_header;
	while (tnd != NULL)
	{
		if (tnd->p != 0) return(tnd->p);
		tnd = tnd->next;
	}
	return(1);
}

char *find_local_name(lkp,ndp)
struct lk_list *lkp;
struct node *ndp;
/*-----------------------------------------------------------------
	Find a qualified name with ndp->l_name as prefix in a syn_list
	(by lkp).
-------------------------------------------------------------------*/
{
	struct lk_list *lk1;
	char buf[MAX_STR_LEN];
	int i; 

	lk1 = lkp;
	while (lk1!=NULL)
	{
		if (lk1->curr_node->type == 's') /* a composite name*/
		{
		i = 0;
		while ((i < strlen(lk1->curr_node->l_name)) &&
			(lk1->curr_node->l_name[i] != '.')) 
		buf[i] = lk1->curr_node->l_name[i++];
		buf[i] = '\0';

		if (strcmp(buf,ndp->l_name) == 0)
			return &lk1->curr_node->l_name[i+1];
		}
		lk1 = lk1->next;
	}
	return "\0";
}

char *find_a_local_name(lkp)
struct lk_list *lkp;
{
	struct lk_list *lk1;

	lk1 = lkp;
	while (lk1 != NULL)
	{
		if (lk1->curr_node->type != 's')
			return lk1->curr_node->l_name;
		lk1 = lk1->next;
	}
	printf("++ CONF. sys error: no logical found in syn_list of(%s)\n",
		lkp->curr_node->l_name);
	exit(-1);
}

char *find_target_e_loc(pt)
struct node *pt;
{
	struct node *t1;
	struct lk_list *tl1;

	tl1 = pt->succ_list;
	while (tl1 != NULL)
	{
		return (tl1->curr_node->e_loc);
/*
		if (tl1->curr_node->P_name[0] != '\0')
			return tl1->curr_node->e_loc;
		tl1 = tl1->next;
*/
	}
	printf("++ CONF. Error: A message queue without a reader? \n");
	printf("exit 1\n");
	exit (1);
}

char *find_target_mail(pt)
struct node *pt;
{
	struct node *t1;
	struct lk_list *tl1;

	tl1 = pt->syn_list;
	while (tl1 != NULL)
	{
		if (tl1->curr_node->P_name[0] != '\0')
			return tl1->curr_node->P_name;
		tl1 = tl1->next;
	}
	return "";
}

/*-----------------------------------------------------------------------*/
clear_screen()
/*
   spawns a process in which an executable image "clear" is run to clear
   the screen
*/
{
   int i;

   i = fork();
   if (i == 0)
   {
      execl("/usr/ucb/clear", "clear", (char *) 0);
   }
   if (i > -1)
      wait(0);
}

/*-----------------------------------------------------------------------*/
yyerror(s)
   char *s;			/* error msg passed by yyparse */

/*
   the error function that the yacc generated parser function yyparse()
   invokes upon a syntax error in the input stream. It outputs the
   remaining unparsed input to the standard output.
*/
{
   int c;

   fprintf(stderr, "++ CONF. ...... %s", s);
   printf("\n\n++ CONF. Unparsed text:");
   printf("\n++ CONF. ------------------\n");

   while ((c = getc(yyin)) != EOF)
   {
      putchar(c);
   }
   exit(PARSE_ERR);
}

generate_inis()
/*
   This routine generates the following:
-	<apps>.objs file for starting object daemons each debugging session.
-	dbx.<program> shell script for each program debugging session.
-	<program>.ini files for program IPC initiailization.
*/
{
   int fd, dbxfd, objfd;
   char name[MAX_STR_LEN], buf[MAX_STR_LEN];
   char def_path[MAX_STR_LEN]; 
   int i, j, fah=0, idx;
   int depen,			/* type of dependency, SEQ or otherwise */
       node_coun,
	fah_cnt,
	fah_nm;		/* restrict fah use < 32 for each activation */
   char name_ar[MAX_PROC + 1][20];	/* logical names in CSL input file */
   int depen_ar[MAX_PROC + 1][MAX_PROC + 1];	/* a matrix to represent
						   the dependencies (a
						   directed graph) where
						   indices of the matrix
						   represent processes
						   with the node_num equal
						   to the index */
   struct lk_list *lklp,
          *lklp2,
          *lklp3;
   struct parm_list *ptr;
   struct node *nodep;

   if (comp_time_debug > 0) 
   printf("\ngenerating .ini files");

   node_coun = 0;
   fah_cnt = 0;
   fah_nm = 0;
   curr_m = m_header;
   while (curr_m != NULL)
   {
      	node_coun++;
	/* Now generate the initialization infor */ 
	if (curr_m->e_loc[0] == 0) strcpy(curr_m->e_loc, dnode); 
	if (!isdigit(curr_m->e_loc[0])) 
		     strcpy(curr_m->e_loc, find_ip_addr(curr_m->e_loc)); 
	if (curr_m->loc[0] == 0) strcpy(curr_m->loc, dnode); 
	if (!isdigit(curr_m->loc[0]))
		     strcpy(curr_m->loc, find_ip_addr(curr_m->loc)); 
   	sprintf(name,"%s.ini", curr_m->l_name);
	if ((fd = creat(name, 0644)) == ERROR)
	{
		perror("\nconf");
		printf("\n++ CONF. couldn't create .ini file\n");
		exit(CONF_ERR);
	}
	write(fd,curr_m->e_loc, strlen(curr_m->e_loc));
	sprintf(name,"\n%s%d\n",dnode,getpid()); 	/* appid */
	write(fd,name,strlen(name));
	sprintf(name,"%s\n",application_name);
	write(fd,name,strlen(name));
	sprintf(name,"%s\n",curr_m->l_name);
	write(fd,name,strlen(name));
	/* This version only supports tcp/ip */
	sprintf(name,"prot = tcp \n");
	write(fd,name,strlen(name));
	sprintf(name,"f = %d\n",curr_m->f); 
	write(fd,name,strlen(name));
	sprintf(name,"P = %d\n", max_no_procs + 1); 
	write(fd,name,strlen(name));
	sprintf(name,"t = %d\n",curr_m->t); 
	write(fd,name,strlen(name));
	sprintf(name,"d = %d\n",curr_m->d); 
	write(fd,name,strlen(name));

	/* Add cpu_list here */
	put_cpu_list(fd);

	/* Generate <program>.dbx shell script */
   	sprintf(name,"%s.dbx", curr_m->l_name);
	if ((dbxfd = creat(name, 0777)) == ERROR)
	{
		perror("\nconf");
		printf("\n++ CONF. couldn't create .dbx file\n");
		exit(CONF_ERR);
	}
	strcpy(name,"#!/bin/sh\n");
	write(dbxfd, name, strlen(name)); 
	strcpy(name,"if [ -f /bin/uname ]; then \n");
	write(dbxfd,name,strlen(name)); 
	strcpy(name,"\tcase \"`/bin/uname`\" in\n"); 
	write(dbxfd,name,strlen(name)); 
	strcpy(name,"\t\t\"OSF1\")\n");
	write(dbxfd,name,strlen(name)); 
	strcpy(name,"\t\t\t os=\"OSF1\"\n");
	write(dbxfd,name,strlen(name)); 
	strcpy(name,"\t\t\t break;\n");
	write(dbxfd,name,strlen(name)); 
	strcpy(name,"\t\t ;;\n");
	write(dbxfd,name,strlen(name)); 
	strcpy(name,"\tesac\n");
	write(dbxfd,name,strlen(name)); 
	strcpy(name,"fi\n");
	write(dbxfd,name,strlen(name)); 
	sprintf(name,"cd %s\n",curr_m->path);
	write(dbxfd,name,strlen(name));
	strcpy(name,"if [ -z \"${os}\"  ]; then\n");
	write(dbxfd,name,strlen(name)); 
	sprintf(name,"\t env CNF_MODULE=%s gdb %s\n", curr_m->l_name,curr_m->P_name);
	write(dbxfd,name,strlen(name));
	strcpy(name,"else\n");
	write(dbxfd,name,strlen(name));
	sprintf(name,"\t env CNF_MODULE=%s gdb %s\n",curr_m->l_name,curr_m->P_name);
	write(dbxfd,name,strlen(name));
	strcpy(name,"fi\n");
	write(dbxfd,name,strlen(name));
	close(dbxfd);

	/* generate command line arguments */
	ptr = curr_m->p_list;
	while (ptr != NULL)
	{
		sprintf(name,"p = %s\n",ptr->parm);
		write(fd,name,strlen(name));
		ptr = ptr->next;
	}
	write(fd,":\n",2);
	lklp = curr_m->pred_list;
	while (lklp != NULL)	
	{
		nodep = lklp->curr_node;
		strcpy(buf,get_local_name(curr_m,lklp->curr_node));
	 	if (!isdigit(nodep->e_loc[0])) 
			strcpy(nodep->e_loc, find_ip_addr(nodep->e_loc)); 
		switch(nodep->org) {
		case SEQ_F: 
			sprintf(name,"f * %s fah%d %s %s/%s %s\n", 
				buf,fah_nm,nodep->e_loc,nodep->e_path,
				nodep->e_P_name,username(nodep->e_loc));
			sprintf(nodep->loc,"fah%d",fah_nm);
			fah_cnt++;
			if (fah_cnt == 32) 
			{
				fah_cnt = 0;
				fah_nm++;
			}
			break;
		case MAIL_F: 
			sprintf(name,"p < %s %s %s %s %s\n",buf,nodep->l_name,
				nodep->e_loc, nodep->P_name,
				username(nodep->e_loc));
			break;
		case TPS: 
			sprintf(name,"t * %s %s %s %s %s\n",buf, 
				nodep->l_name, nodep->e_loc, 
				nodep->P_name, username(nodep->e_loc));
			break; 
	 	}
		write(fd,name,strlen(name)); 
		lklp = lklp->next;
	}
	lklp = curr_m->succ_list;
	while (lklp != NULL)	
	{
		nodep = lklp->curr_node;
		strcpy(buf,get_local_name(curr_m,nodep));
		if (!isdigit(nodep->e_loc[0]))
			strcpy(nodep->e_loc, find_ip_addr(nodep->e_loc)); 
		switch(nodep->org) {
		case SEQ_F: 
	 		sprintf(name,"f * %s fah%d %s %s/%s %s\n", 
				buf, fah_nm, nodep->e_loc,
				nodep->e_path, nodep->e_P_name,
				username(nodep->e_loc));
			sprintf(nodep->loc,"fah%d",fah_nm);
			fah_cnt++;
			if (fah_cnt == 32) 
			{
				fah_cnt = 0;
				fah_nm++;
			}
			break;
	 	case MAIL_F:
			sprintf(name,"p > %s %s %s %s %s\n",
					 buf, nodep->l_name,
					nodep->e_loc, nodep->P_name,
					username(nodep->e_loc)); 
          		nodep->temp--;  
			break;
	 	case TPS:
			sprintf(name,"t * %s %s %s %s %s\n", 
					buf, nodep->l_name,
					nodep->e_loc, nodep->P_name,
					username(nodep->e_loc));
			break;
		}
	 	write(fd,name,strlen(name));
		lklp = lklp->next;
	}
	close(fd);
	curr_m = curr_m->next;
   }

   /* Generate <application>.objs.prcd */
   sprintf(name,"%s.objs.prcd",application_name);
   if ((fd = creat(name, 0644)) == ERROR)
	{
		perror("\nconf");
		printf("\n++ CONF. couldn't create .objs file\n");
		exit(CONF_ERR);
	}
   sprintf(name,"%s\n",application_name);
   write(fd,name, strlen(name));
   strcpy(name,"debu\n");
   write(fd,name, strlen(name));
   write(fd,"@\n",2);
   /* Generate Handlers */
   for (i=0; i<= fah_nm; i++)
   {
	sprintf(name,"fah%d",i);
   	curr_f = f_header;
   	while (curr_f != NULL)
   	{
		if ((curr_f->org == SEQ_F) && (!strcmp(curr_f->loc,name))) 
		{
			sprintf(name,"f %s %s %s %s%d\n",
				curr_f->loc, curr_f->e_loc,
				username(curr_f->e_loc), dnode, getpid());
			write(fd,name,strlen(name));
		}
		curr_f = curr_f ->next;
   	}
   }
   curr_f = f_header;
   while (curr_f != NULL)
   {
	if (curr_f->org == TPS)
	{
		sprintf(name,"t %s %s %s %s%d\n",
			curr_f->l_name, curr_f->e_loc, 
			username(curr_f->e_loc),dnode,getpid());
		write(fd,name,strlen(name));
	}
	curr_f = curr_f ->next;
   }
   write(fd,"-\n", 2);
   close(fd);

   /* Generate <application>.objs */
   sprintf(name,"%s.objs",application_name);
   if ((fd = creat(name, 0644)) == ERROR)
	{
		perror("\nconf");
		printf("\n++ CONF. couldn't create .objs file\n");
		exit(CONF_ERR);
	}
   for (i=0; i<= fah_nm; i++)
   {
	sprintf(name,"fah%d",i);
   	curr_f = f_header;
   	while (curr_f != NULL)
   	{
		if ((curr_f->org == SEQ_F) && (!strcmp(curr_f->loc,name))) 
		{
			idx = find_host_idx(curr_f->e_loc);
			sprintf(name,"%s %s fah %s%d %s %s\n",
				ary_node_cap[idx].ipaddr,ary_node_cap[idx].name,
				dnode, getpid(),
				curr_f->loc, ary_node_cap[idx].user_name);
			write(fd,name,strlen(name));
		}
		curr_f = curr_f ->next;
   	}
   }
   curr_m = f_header;
   while (curr_m != NULL)
   {
		idx = find_host_idx(curr_m->e_loc);
		switch(curr_m->org) {
		case TPS:
				sprintf(name,"%s %s tsh %s%d %s %s\n",
					ary_node_cap[idx].ipaddr,
					ary_node_cap[idx].name,
					dnode,
					getpid(),
					curr_m->l_name,
					ary_node_cap[idx].user_name);
				write(fd,name,strlen(name));
				break;
		} 
		curr_m = curr_m->next;
   }
   close(fd);

   /* Generate <application>.end */
   sprintf(name,"%s.end",application_name);
   if ((fd = creat(name, 0777)) == ERROR)
	{
		perror("\nconf");
		printf("\n++ CONF. couldn't create .end file\n");
		exit(CONF_ERR);
	}
   sprintf(name,"#!/bin/sh\n");
   write(fd,name,strlen(name));
   sprintf(name,"kobjs %s.objs\n",application_name);
   write(fd,name,strlen(name));
   sprintf(name,"/bin/rm %s.objs\n",application_name);
   write(fd,name,strlen(name));
   sprintf(name,"/bin/rm %s.objs.prcd\n",application_name);
   write(fd,name,strlen(name));
   sprintf(name,"/bin/rm *.ini\n");
   write(fd,name,strlen(name));
   sprintf(name,"/bin/rm *.dbx\n");
   write(fd,name,strlen(name));
   /* 
   curr_m = m_header;
   while (curr_m != NULL)
   {
	sprintf(name,"/bin/rm %s.ini\n",curr_m->l_name);
	write(fd,name,strlen(name));
	sprintf(name,"/bin/rm %s.dbx\n",curr_m->l_name);
	write(fd,name,strlen(name));
	curr_m = curr_m->next;
   }
   */
   sprintf(name,"echo [%s] Debugging Session End.\n",application_name);
   write(fd,name,strlen(name));
   sprintf(name,"/bin/rm %s.end\n",application_name);
   write(fd,name,strlen(name));
   close(fd);
}	/* end of generate_inis */

/*-----------------------------------------------------------------*/
generate_prcd()
/*
   evaluates the process execution precedence and creates a file which has
   the processes listed by logical names, exec names and exec locations
   and has the interprocess dependencies in the form similar to a matrix.
   The processes are numbered and if process j receives from a DS (data
   store) that process i outputs to, then i is listed on the jth row. If
   any of the datastores are sequential, then -i is listed to indicate a
   strict sequencing. The file is created with the application name and a
   .prcd extension.
*/
{
   int fd;
   char name[MAX_STR_LEN], buf[MAX_STR_LEN];
   char def_path[MAX_STR_LEN]; 
   int i, j, fah=0;
   int depen,			/* type of dependency, SEQ or otherwise */
       node_coun, fah_cnt, fah_nm;
   char name_ar[MAX_PROC + 1][20];	/* logical names in CSL input file */
   int depen_ar[MAX_PROC + 1][MAX_PROC + 1];	/* a matrix to represent
						   the dependencies (a
						   directed graph) where
						   indices of the matrix
						   represent processes
						   with the node_num equal
						   to the index */
   struct lk_list *lklp,
          *lklp2,
          *lklp3;
   struct parm_list *ptr;

   for (i = 0; i <= MAX_PROC; i++)
   {
      name_ar[i][0] = '\0';
      for (j = 0; j <= MAX_PROC; j++)
	 depen_ar[i][j] = 0;
   }

   if (comp_time_debug > 0) 
   printf("\ngenerating .prcd file");

   strcpy(name, application_name);
   strcpy(&name[strlen(name)], ".prcd");

   if ((fd = creat(name, 0644)) == ERROR)
   {
      perror("\nconf");
      printf("\n++ CONF. couldn't create .prcd file\n");
      exit(CONF_ERR);
   }
   sprintf(name,"%s\n",application_name);
   write(fd,name,strlen(name));
/*
   sprintf(name,"debug = %d\n",run_time_debug); 
   write(fd,name,strlen(name));
*/
   node_coun = 0;
   fah_cnt = 0;
   fah_nm = 0;
   curr_m = m_header;
   while (curr_m != NULL)
   {
   	write(fd,"\%\n",2);
      	node_coun++;
	if (curr_m->e_loc[0] == 0) strcpy(curr_m->e_loc, dnode); 
	if (!isdigit(curr_m->e_loc[0])) 
		     strcpy(curr_m->e_loc, find_ip_addr(curr_m->e_loc)); 
	if (curr_m->loc[0] == 0) strcpy(curr_m->loc, dnode); 
	if (!isdigit(curr_m->loc[0]))
		     strcpy(curr_m->loc, find_ip_addr(curr_m->loc)); 
	sprintf(buf,"%s\n",curr_m->l_name);
	write(fd,buf,strlen(buf));
	sprintf(buf,"%s %s/%s %s\n",curr_m->loc, curr_m->path,
		curr_m->P_name,username(curr_m->loc));
	write(fd,buf,strlen(buf));
	sprintf(buf,"%s %s/%s %s\n",curr_m->e_loc, curr_m->e_path, 
		curr_m->P_name, username(curr_m->e_loc));
	write(fd,buf,strlen(buf));
	/* This version only supports tcp/ip */ 
	sprintf(name,"prot = tcp \n");
	write(fd,name,strlen(name));
	sprintf(name,"f = %d\n",curr_m->f);
	write(fd,name,strlen(name));
	sprintf(name,"p = %d\n", max_no_procs + 1);
	write(fd,name,strlen(name));
	sprintf(name,"t = %d\n", curr_m->t);
	write(fd,name,strlen(name));
	sprintf(name,"d = %d\n", curr_m->d);
	write(fd,name,strlen(name));
	/* generate command line arguments */
	ptr = curr_m->p_list;
	while (ptr != NULL)
	{
		sprintf(name,"%s\n",ptr->parm);
		write(fd,name,strlen(name));
		ptr = ptr->next;
	}
	write(fd,"!\n",2);
	lklp = curr_m->pred_list;
	while (lklp != NULL)	
	{
		struct node *nodep;

		nodep = lklp->curr_node;
		strcpy(buf,get_local_name(curr_m,lklp->curr_node));
	 	if (!isdigit(nodep->e_loc[0]))
			strcpy(nodep->e_loc,find_ip_addr(nodep->e_loc)); 
		switch(nodep->org) {
		case SEQ_F: 
			sprintf(nodep->loc,"fah%d",fah_nm);
			sprintf(name,"f * %s %s %s/%s %s\n", 	
				buf,nodep->loc,nodep->e_path,
				nodep->e_P_name,nodep->mode);
			fah_cnt++;
			if (fah_cnt == 32) 
			{
				fah_nm++;
				fah_cnt = 0;
			}
			break;
		case MAIL_F: 
			sprintf(name,"p < %s %s\n",buf,nodep->l_name);
			break;
		case TPS: 
			sprintf(name,"t * %s %s\n",buf, 
				nodep->l_name); 
			break; 
	 	}
		write(fd,name,strlen(name)); 
		lklp = lklp->next;
	}
	lklp = curr_m->succ_list;
	while (lklp != NULL)	
	{
		struct node *nodep;

		nodep = lklp->curr_node;
		strcpy(buf,get_local_name(curr_m,nodep));
		if (!isdigit(nodep->e_loc[0]))
			strcpy(nodep->e_loc,find_ip_addr(nodep->e_loc)); 
		switch(nodep->org) {
		case SEQ_F: 
			sprintf(nodep->loc,"fah%d",fah_nm);
	 		sprintf(name,"f * %s %s %s/%s %s\n", 
				buf, nodep->loc,
				nodep->e_path,nodep->e_P_name,
				nodep->mode);
			fah_cnt++;
			if (fah_cnt == 32) 
			{
				fah_nm++;
				fah_cnt = 0;
			}
			break;
	 	case MAIL_F:
			sprintf(name,"p > %s %s\n",
					 buf, nodep->l_name); 
          		nodep->temp--;  
			break;
	 	case TPS:
			sprintf(name,"t * %s %s\n", 
					buf, nodep->l_name); 
			break;
		}
	 	write(fd,name,strlen(name));
		lklp = lklp->next;
	}
	/* Prepare dependency marks */ 
	i = curr_m->node_num;
	strcpy(name_ar[i], curr_m->l_name);
	lklp = curr_m->pred_list;
	while (lklp != NULL)	
	{ 	if (lklp->curr_node->org == SEQ_F) depen = -1;	
		else depen = 1;	
		lklp2 = lklp->curr_node->syn_list;
	 	while (lklp2 != NULL)
	 	{  	lklp3 = lklp2->curr_node->pred_list;
			while (lklp3 != NULL)	
	    		{	j = lklp3->curr_node->node_num;	
	       			if (depen_ar[i][j] > -1)	
		  			depen_ar[i][j] = depen;	
	       			lklp3 = lklp3->next;
	   		}
	    		lklp2 = lklp2->next;
		}
	 	lklp = lklp->next;
     	}
	write(fd,":\n",2);
	curr_m = curr_m->next;
   }
   write(fd,"@\n",2);
   /* Generate Handlers */
   for (i=0; i<= fah_nm; i++)
   {
	sprintf(name,"fah%d",i);
   	curr_f = f_header;
   	while (curr_f != NULL)
   	{
		if ((curr_f->org == SEQ_F) && (!strcmp(curr_f->loc,name))) 
		{
			sprintf(name,"f %s %s %s\n",
				curr_f->loc, curr_f->e_loc,
				username(curr_f->e_loc));
			write(fd,name,strlen(name));
		}
		curr_f = curr_f ->next;
   	}
   }
   curr_f = f_header;
   while (curr_f != NULL)
   {
	if (curr_f->org == TPS)
	{
		sprintf(name,"t %s %s %s\n",
			curr_f->l_name, curr_f->e_loc, 
			username(curr_f->e_loc));
		write(fd,name,strlen(name));
	}
	curr_f = curr_f ->next;
   }
   write(fd,"-\n", 2);
   /* Normal modules */ 
   for (i = 1; i <= node_coun; i++)	/* for each process */
   {
      if (comp_time_debug > 0) 
      {
	printf("\n%s ", name_ar[i]); 
 	for (j = 1; j <= node_coun; j++)
 	printf(" %d", depen_ar[i][j]);
      } 
      sprintf(name, "%s", name_ar[i]);	 /* begin row with module
						   number and name */ 
      strcat(name,":");
      write(fd, name, strlen(name));
      for (j = 1; j <= node_coun; j++)	
	 if (depen_ar[i][j] != 0)
	 {
		if (depen_ar[i][j] < 0 )
		{
	    		sprintf(name, " %s", name_ar[j]);
	    		write(fd, name, strlen(name));
		}
	 }
      write(fd, "\n", strlen("\n"));
   }
   close(fd);
}

char *get_sender_cpu(np)
struct node *np;
{
	struct lk_list *tl1;

	tl1 = np->pred_list;
 	while( tl1 != NULL)
	{
		if (tl1->curr_node->type == 'M')
		return tl1->curr_node->e_loc; 
		tl1 = tl1->next;
	}
	return "\n";
}

char *username(name)
char *name;
{
	int i;
	for (i=0; i<=max_no_procs; i++)
	{
		if (strcmp(ary_node_cap[i].name,name)==0)
			return ary_node_cap[i].user_name;
		if (strcmp(ary_node_cap[i].ipaddr,name)==0)
			return ary_node_cap[i].user_name;
	}
	return "NA";
}

char *find_ip_addr(name)
char *name;
{
	int i;

	if (strlen(name) == 0) return "No Ip addr";
	for (i=0; i<=max_no_procs; i++)
	{
		if ((strcmp(ary_node_cap[i].name,name)==0) ||
		    (strcmp(strip_dot(ary_node_cap[i].name),name)==0))
		{
			return ary_node_cap[i].ipaddr;
		} 
	}
	return "NA";
}

int find_host_idx(name)
char *name;
{
	int i;
	for (i=0; i<=max_no_procs; i++)
	{
		if ((strcmp(ary_node_cap[i].ipaddr,name)==0) ||
		    (strcmp(ary_node_cap[i].name,name)==0) ||
		    (strcmp(strip_dot(ary_node_cap[i].name),name)==0))
		{
			return(i);
		} 
	}
	return(-1);
}

/* Returns a cpu upon request. Recycle cpu list if more programs are found. */
char *get_a_cpu(cnt, type, fsys) 
int cnt;  /* 0 | 1 */
int type; /* 1: slave processor; 0: others */
char *fsys; /* file system mask */
{
	int i;
	char ip_mask[128];
	
re_cycle:
	for (i=0; i<=max_no_procs; i++)
	{
		if (ary_node_cap[i].used == re_cycle_no) 
		{
			if (type == 1) /* slave processor. only do same fsys */
			{
				if (!strcmp(ary_node_cap[i].fsys, fsys))
				{ 
			ary_node_cap[i].used = ary_node_cap[i].used + cnt;
			return ary_node_cap[i].name;
				}
			} else {
			ary_node_cap[i].used = ary_node_cap[i].used + cnt;
			return ary_node_cap[i].name;
			}
		}
	}
	if (re_cycle_no == 0) /* warning only the first time */
	{
	  printf("++ CONF. Warning: \n");
 	  printf("++      Processor recycling. Expect low efficiency ...\n");
	}
	re_cycle_no ++;
	goto re_cycle;
}
	
char *strip_dot(name)
char *name;
{
	int i;
	char buf[128];

	for (i=0; i<strlen(name); i++)
	{
		if (name[i] == '.')
		{
			name[i] = '\0';
			return(name);
		}
	}
	return(name);
}


generate_cpu_list() 	/* For dynamically setup daemons. Y.S. 94 */
{
	int i;
	FILE *fd;
	char filenm[128];

	sprintf(filenm,"%s.cpus",application_name);
	if (!(fd = fopen(filenm,"w")))
	{
		perror("Cannot write cpu list file");
		exit(1);
	}
	for (i=0; i<=max_no_procs; i++)
	{
		if (ary_node_cap[i].used == 1)
		{ 
			fprintf(fd,"%s %s %s %s %s\n",
			ary_node_cap[i].ipaddr,
			ary_node_cap[i].name,
			ary_node_cap[i].prot,
			ary_node_cap[i].OS,
			ary_node_cap[i].user_name);
		}
	}
	fclose(fd);
}

put_cpu_list(fd)
int fd;
{
	int i, cnt=0;
	char buf[128];

	for (i=0; i<=max_no_procs; i++)
		if (ary_node_cap[i].used == 1) cnt++;
	sprintf(buf, "%d\n", cnt);
	write(fd,buf,strlen(buf));
	for (i=0; i<=max_no_procs; i++)
	{
		if (ary_node_cap[i].used == 1)
		{ 
			sprintf(buf,"%s %s\n",
			(ary_node_cap[i].ipaddr),
			ary_node_cap[i].user_name);
			write(fd, buf, strlen(buf));
		}
	}
}

char *get_local_name(m_p,fnp)
struct node *m_p,*fnp;
{
	char buf[MAX_STR_LEN];

	if (strlen(find_local_name(fnp->syn_list,m_p)) == 0) return find_a_local_name(fnp->syn_list,m_p);
	else return(find_local_name(fnp->syn_list,m_p));
}


build_net_struct()
{
   FILE   *fd;
   int  rtn_val=0,sw=0, cpu_cnt=0;  
   char buf[PHYS_ID_LEN];

 strcpy(buf,".snghosts");
 if((fd=fopen(buf,"r")) !=NULL)
  {
	sw = 0;
	rtn_val = fscanf(fd,"%s %s %s %s %s %s",
		ary_node_cap[sw].ipaddr,
		ary_node_cap[sw].name,
		ary_node_cap[sw].prot,
		ary_node_cap[sw].OS,
		ary_node_cap[sw].user_name,
		ary_node_cap[sw].fsys);
	while (rtn_val > 0)
	{
		cpu_cnt++;
/*
		if (cpu_cnt > 3)
		{
			printf(" CPU Limit Exceeded. This Evaluation Version Only Allows at Most Three (3) Processors. \n");
			exit(1);
		}
*/
		ary_node_cap[sw].used = 0;
		sw++;
		rtn_val = fscanf(fd,"%s %s %s %s %s %s",
			ary_node_cap[sw].ipaddr,
			ary_node_cap[sw].name,
			ary_node_cap[sw].prot,
			ary_node_cap[sw].OS,
			ary_node_cap[sw].user_name,
			ary_node_cap[sw].fsys);
	}
	cnt = sw-1;
	max_no_procs = cnt;
	for(sw=0;sw<=cnt;sw++)
	{
/*
		if (!isdigit(ary_node_cap[sw].name[0])) 
		{
		     if ((host=gethostbyname(ary_node_cap[sw].name)) == NULL) 
		     {	printf("++ CONF. Invalid host(%s)\n",ary_node_cap[sw].name); 
		      		perror("Invalid Hostname Found");
				exit(1);
		     }
		     dest.sin_family = host->h_addrtype;
		     bcopy(host->h_addr, (caddr_t) & dest.sin_addr,
				host->h_length);
		     destdotaddr = inet_ntoa(dest.sin_addr);
		     strcpy(ary_node_cap[sw].name, destdotaddr); 
		} 
*/
		if (comp_time_debug > 0)
		{
		printf("%15s %15s %5s %5s %10s %s\n", ary_node_cap[sw].ipaddr,
			ary_node_cap[sw].name, 
			ary_node_cap[sw].prot,
			ary_node_cap[sw].OS,
			ary_node_cap[sw].user_name,
			ary_node_cap[sw].fsys); 
		}		
	}
  } else {
	printf("++ CONF. Cannot find .snghosts file. Please run cds -f first.\n");
	exit(1);
  }
}

show_graph()
{
   struct parm_list *ptr;
   tnd = m_header;
   printf(" Show parsed data. M nodes:::\n");
   while( tnd != NULL)
   {
	printf("===============================(%d)\n",tnd->node_num);
	printf("  l_name = (%s) \n",tnd->l_name);
	printf("  type   = (%c) \n",tnd->org);
	printf("  P_name = (%s) \n",tnd->P_name);
	printf("  loc    = (%s) \n",tnd->loc);
	printf("  e_loc  = (%s) \n",tnd->e_loc);
	printf("  e_path = (%s) \n",tnd->e_path);
	printf("  e_P_name=(%s) \n",tnd->e_P_name);
	printf("  p =(%d)\n",tnd->p);
	printf("  f =(%d)\n",tnd->f);
	printf("  d =(%d)\n",tnd->d);
	printf("  t =(%d)\n",tnd->t);
	printf(" \n Preds: \n");
	tlk = tnd->pred_list;
	if (tlk == NULL) printf(" [nil]\n");
	while (tlk != NULL)
	{
		printf("   l_name(%s)\n",tlk->curr_node->l_name);
		printf("   P_name)%s)\n",tlk->curr_node->P_name);
		tlk = tlk->next;
	}
	printf(" \n Succs: \n");
	tlk = tnd->succ_list;
	if (tlk == NULL) printf(" [nill]\n");
	while (tlk != NULL) 
	{
		printf("   l_name(%s)\n",tlk->curr_node->l_name);
		printf("   P_name(%s)\n",tlk->curr_node->P_name);
		tlk = tlk->next; 
	}
	printf(" \n Syns: \n");
	tlk = tnd->syn_list;
	if (tlk == NULL) printf(" [nil]\n");
	while (tlk != NULL)
	{
		printf("  [%s]",tlk->curr_node->l_name);
		tlk = tlk->next;
	}
	printf(" Parameters: \n");
	ptr = tnd->p_list;
	while (ptr != NULL)
	{
		printf("   (%s)\n",ptr->parm);
		ptr = ptr->next;
	}
	printf("\n");
	tnd = tnd->next;
   }

   tnd = f_header;
   printf(" Show parsed data. F nodes:::\n");
   while( tnd != NULL)
   {
	printf("FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF(%d)\n",tnd->node_num);
	printf("  l_name = (%s) \n",tnd->l_name);
	printf("  org    = (%c) \n",tnd->org);
	printf("  P_name = (%s) \n",tnd->P_name);
	printf("  loc    = (%s) \n",tnd->loc);
	printf("  e_loc  = (%s) \n",tnd->e_loc);
	printf("  e_P_name=(%s) \n",tnd->e_P_name);
	printf("  mode =(%s)\n", tnd->mode);
	printf(" \n Preds: \n");
	tlk = tnd->pred_list;
	if (tlk == NULL) printf(" [nil]\n");
	while (tlk != NULL)
	{
		printf("   l_name(%s)\n",tlk->curr_node->l_name);
		printf("   P_name)%s)\n",tlk->curr_node->P_name);
		tlk = tlk->next;
	}
	printf(" \n Succs: \n");
	tlk = tnd->succ_list;
	if (tlk == NULL) printf(" [nill]\n");
	while (tlk != NULL) 
	{
		printf("   l_name(%s)\n",tlk->curr_node->l_name);
		printf("   P_name(%s)\n",tlk->curr_node->P_name);
		tlk = tlk->next; 
	}
	printf(" \n Syns: \n");
	tlk = tnd->syn_list;
	if (tlk == NULL) printf(" [nil]\n");
	while (tlk != NULL)
	{
		printf("  [%s]",tlk->curr_node->l_name);
		tlk = tlk->next;
	}
	printf("\n");
	tnd = tnd->next;
   }
} /* of show_graph() */

/* 
    generate network information
*/

int   i,j ;
int fd; 
char  buf[MAX_STR_LEN],buf1[MAX_STR_LEN],tag[MAX_STR_LEN],name[MAX_STR_LEN];
struct lk_list   *lklp;
struct  node   *nodep, *t_node; 

generate_net_doc()
{
   strcpy(buf,application_name);
   strcat(&buf[strlen(buf)],".doc");
   if((fd= creat(buf,0644)) == ERROR)
   {
     printf("++ CONF. couldn't open file: %s %d \n",buf, fd);
     perror(" couldn't open report file \n"); 
     exit(-1);
   }  
   put_under_line();
   sprintf(buf,"                      SUMMARY OF CONFIGURATION\n");
   write(fd,buf,strlen(buf));
   put_under_line(); 
   generate_prefix_doc(); 
   generate_matrix_doc();
   generate_module_doc();
   generate_file_doc(); 
   close(fd); 
}

generate_prefix_doc()
{
    int  fd1,fd2,cnt1, cnt2; 

    sprintf(buf,"\nThe  application  name  is  : (%s)\n",application_name);
    write(fd,buf,strlen(buf));
    strcpy(buf1,application_name);
    strcat(buf1,".prcd");
    sprintf(buf,"Precedence relation file is : (%s)\n",buf1);
    write(fd,buf,strlen(buf));
    sprintf(buf,"\nDate/Time of Compilation is : ");
    write(fd,buf,strlen(buf));
    fd1=dup(1); close(1); 
    dup(fd);
    system("date");
    close(1); dup(fd1);
    close(fd1);
    write(fd,"\n\n",strlen("\n\n"));  
    cnt1=0; cnt2=0; buf[0]='\0';  curr_m= m_header;
    while(curr_m != NULL)
    {
      cnt1++;
      if(strncmp(buf,curr_m->P_name,strlen(curr_m->P_name)) !=0)
      {
         strcpy(buf,curr_m->P_name);
         cnt2++;
      }
      curr_m = curr_m->next; 
    }
    sprintf(buf,"Total # of Logical Modules  : %d\n",cnt1);
    write(fd,buf,strlen(buf)); 
    sprintf(buf,"Total # of Physical Modules : %d\n\n",cnt2);
    write(fd,buf,strlen(buf));
    cnt1=0; cnt2=0, buf[0]='\0'; curr_f= f_header;
    while(curr_f != NULL)
    {
      cnt1++; 
      tnd= f_header; 
      while(tnd != curr_f)
      {
         lklp= tnd->syn_list;  
         while( lklp !=NULL)
         {
           nodep= lklp->curr_node ;
           if(strcmp(nodep->l_name,curr_f->l_name) ==0) goto loop ;
           lklp= lklp->next; 
         };
         tnd= tnd->next;
      };
      cnt2++;
loop: curr_f= curr_f->next;
    }
    sprintf(buf,"Total # of Logical Files    : %d\n",cnt1);
    write(fd,buf,strlen(buf));
    sprintf(buf,"Total # of Physical Files   : %d\n\n",cnt2);
    write(fd,buf,strlen(buf));
 
        
}

    /* 
       show all modules' infoormation  
       write modules' name with their physical location, execution location, and
       the executed module's control file 
    */ 
generate_module_doc()
{
    int len_cnt ,t ; 


    curr_m = m_header; i=1;  
    put_under_line();                     
    sprintf(buf,"                      Program Objects:\n");
    write(fd,buf,strlen(buf)); 
    put_under_line();  write(fd,"\n",strlen("\n"));
    while(curr_m != NULL)
    {
      sprintf(buf,"%d [%s] ",i++,curr_m->l_name);
      write(fd,buf,strlen(buf));
      sprintf(buf,"Storage location  : (%s)\n",curr_m->loc);
      write(fd,buf,strlen(buf));
      sprintf(buf,"          Execution location: (%s)\n",
                            curr_m->e_loc); 
      write(fd,buf,strlen(buf)); 
      strcpy(buf1,curr_m->l_name);
      strcat(buf1,".ini"); 
      sprintf(buf,"  Debugging IPC pattern file: (%s)\n",buf1);
      write(fd,buf,strlen(buf));
      sprintf(buf,"          Input/Output:\n");
      write(fd,buf,strlen(buf)); 
      lklp= curr_m->pred_list;
      write(fd,"              Input  objects: ",30);
      len_cnt =30;  
      while(lklp != NULL)
      {
        nodep = lklp -> curr_node;
        len_cnt= len_cnt+ strlen(nodep->l_name)+ strlen(nodep->P_name)+5;
        if( len_cnt > 80) {
                            write(fd,"\n",strlen("\n"));
                            for(t=0;t<30;t++) write(fd," ",1);  
                            len_cnt= 30;
                          } ; 
        sprintf(buf1," %s (%s) " , nodep->l_name, nodep->P_name);
        write(fd,buf1,strlen(buf1));
        lklp = lklp -> next;
      };
      write(fd,"\n",strlen("\n"));
      len_cnt =30;  
      write(fd,"              Output Objects: ",30); 
      lklp= curr_m-> succ_list;
      while( lklp != NULL)
      {
         nodep = lklp -> curr_node; 
         len_cnt= len_cnt+strlen(nodep->l_name)+strlen(nodep->P_name)+5; 
         if(len_cnt>80) {
                           write(fd,"\n",strlen("\n")); 
                           for(t=0;t<30;t++) write(fd," ",1);
                           len_cnt= 30;
                        }; 
         sprintf(buf1," %s (%s) ",nodep->l_name, nodep->P_name);
         write(fd,buf1,strlen(buf1));
         lklp =lklp -> next;  
      };
      write(fd,"\n\n",strlen("\n\n")); 
      curr_m= curr_m->next;
    };
    write(fd,"\n\n",strlen("\n\n"));  
   }

generate_file_doc()
{

    /* show all data stores information */

    put_under_line();
    sprintf(buf,"                        Data Ojects:\n");
    write(fd,buf,strlen(buf)); 
    put_under_line();  write(fd,"\n",strlen("\n")); 

    curr_f = f_header;
    while( curr_f != NULL)
    { 
        sprintf(buf,"[%s]\n",curr_f->l_name);
        write(fd,buf,strlen(buf));
        sprintf(buf,"          type            : %c\n",curr_f->type);
        write(fd,buf,strlen(buf));
        sprintf(buf,"          storage location: (%s)\n",curr_f->loc); 
        write(fd,buf,strlen(buf));
        sprintf(buf,"          physical name   : (%s)\n",curr_f->P_name);
        write(fd,buf,strlen(buf));
        sprintf(buf,"          object name     : (%s)\n",curr_f->l_name); 
        write(fd,buf,strlen(buf)); 
        sprintf(buf,"          reference names :\n"); 
        write(fd,buf,strlen(buf)); 
        lklp = curr_f-> pred_list ;  
        while(lklp != NULL) 
        {
           nodep = lklp -> curr_node;
                                         /* if it is F->F situation then skip it
                                             else print out logical name  */ 
           if(check_data_store(nodep->l_name)== 't') goto loop1; 
           strcpy(buf1,get_local_name(nodep,curr_f)); 
           sprintf(buf,"		%s.%s(v)\n",nodep->l_name,buf1); 
           write(fd,buf,strlen(buf));  
      loop1: lklp = lklp->next; 
        };
        lklp= curr_f->succ_list ; 
        while(lklp != NULL)
        { 
           nodep = lklp ->curr_node; 
           if(check_data_store(nodep->l_name ) == 't') goto loop2;  
           strcpy(buf1,get_local_name(nodep,curr_f)); 
           sprintf(buf,"		%s.%s(^)\n",nodep->l_name,buf1 );  
           write(fd,buf,strlen(buf)); 
      loop2: lklp = lklp->next; 
        };
       curr_f = curr_f->next; 
    };
    /*  show all available nodes information with their name, os, protocol, and
        pathname if mentioned
    */
    write(fd,"\n\n",strlen("\n\n"));  
    put_under_line(); 
    sprintf(buf,"                     Execution Environment \n");
    write(fd,buf,strlen(buf));
    put_under_line() ; write(fd,"\n",strlen("\n")); 
    i=0;  
    sprintf(buf,"The default node is      : (%s)\n",dnode); 
    write(fd,buf,strlen(buf));
    sprintf(buf,"The default directory is : (%s)\n\n",pwd);
    write(fd,buf,strlen(buf));  
    while(ary_node_cap[i].name[0] !='\0')
    {
      sprintf(buf,"node_name        : %s\n",ary_node_cap[i].name);
      write(fd,buf,strlen(buf)); 
      i++ ;
    };
    write(fd,"\n\n",strlen("\n\n"));
}

check_data_store(s) 
char *s;
{
  tnd= f_header;
  while(tnd !=NULL)
  {
     if(strcmp(tnd->l_name,s)==0)  return('t');
     tnd= tnd->next;
  };
  return('f');
}

struct  matrix
        {
          int    module_num ;
          char   name_dir[MAX_STR_LEN];
          struct matrix  *next;
          struct matrix  *succ;
        };

struct  matrix  *mtx_header=NULL, *curr_mtx= NULL;
int count=1;
char *find_local_file_name() ; 


generate_matrix_doc( )

{
   char  *mark, ch ;

 
   curr_m= m_header;
   while( curr_m !=NULL)
   {
      lklp= curr_m->pred_list;
      while(lklp !=NULL)
      {
         nodep= lklp ->curr_node;
         if(nodep->P_name[0] != '\0') 
         { 
         search_old_file(nodep->P_name);
         mark= "v"  ;
         move_mark_into_matrix(curr_mtx,mark);
         }
         lklp= lklp->next;
      };
      lklp= curr_m->succ_list;
      while(lklp !=NULL)
      {
         nodep= lklp->curr_node;
         if(nodep->P_name[0] !='\0')
         {
         search_old_file(nodep->P_name);
         mark= "^" ;
         move_mark_into_matrix(curr_mtx,mark);
         }
         lklp= lklp->next;
      };
      count++;
      curr_m= curr_m->next;
   }
    print_message();
}


search_old_file(s)
char *s;
{  
   struct matrix *p;

   if(mtx_header == NULL)
   {
      mtx_header= (struct matrix *)malloc(sizeof(struct matrix));
      mtx_header->next= NULL;
      mtx_header->succ= NULL;
      strncpy(mtx_header->name_dir,s,strlen(s));
      mtx_header->module_num= 0;
      curr_mtx= mtx_header;
   }
   else
   {
      p= mtx_header;
      while(p !=NULL && strcmp(p->name_dir,s) !=0 )
      { curr_mtx= p;
        p= p->next;
      }
      if(p == NULL)
      {
         curr_mtx->next= (struct matrix *) malloc(sizeof(struct matrix));
         curr_mtx= curr_mtx->next;
         curr_mtx->next= NULL;
         curr_mtx->succ= NULL; 
         strncpy(curr_mtx->name_dir,s,strlen(s));
         curr_mtx->module_num= 0; 
      }
      else  curr_mtx= p; 
   }
}

move_mark_into_matrix(p,s)
struct matrix *p;
char *s;
{
   struct matrix *ptr;
   
 
   if((p->succ) !=NULL)
   {
      ptr = p->succ;
      while(ptr !=NULL)
      {  p= ptr;
         ptr= ptr ->succ;
      } 
   }
   p->succ=(struct matrix *) malloc(sizeof(struct matrix));
   p= p->succ;
   p->next= NULL; 
   p->succ= NULL; 
   strncpy(p->name_dir,s,strlen(s));
   p->module_num = count;
}

print_message()
{
  struct  matrix *p1, *p2;
  int cnt_num=1, index  ;


  curr_m= m_header; 
  put_under_line();  
  sprintf(buf,"          		IPC Pattern:\n"); 
  write(fd,buf,strlen(buf)); 
  put_under_line();  write(fd,"\n",strlen("\n")); 
  p1= mtx_header; index=1; 
  while(p1 !=NULL)
  {
   sprintf(buf,"%d =  %s (%s)\n",index++,
             p1->name_dir,find_local_file_name(p1->name_dir));
   write(fd,buf,strlen(buf)); 
   p1= p1->next;
  };
  put_under_line();  
  write(fd,"module\\files  ",strlen("module\\files  "));
  for(j=1;j<index;j++) 
   {
      sprintf(buf,"%d     ",j) ;
      write(fd,buf,strlen(buf));
   };
  write(fd,"\n",strlen("\n"));
 
  while(curr_m !=NULL) 
  {
    sprintf(buf,"\n[%s]       ",curr_m->l_name); 
    write(fd,buf,strlen(buf)); 
    p2= mtx_header; 
lp: p1= p2->succ; 
    while( p1 !=NULL && p1->module_num !=cnt_num ) p1= p1->succ;  
    if(p1 == NULL) sprintf(buf,"      ");
    else  sprintf(buf,"%s     ",p1->name_dir);  
    write(fd,buf,strlen(buf));
    p2= p2->next;
    if(p2 !=NULL) goto lp;
    cnt_num++; 
    curr_m= curr_m->next; 
  };
  write(fd,"\n\n\n",strlen("\n\n\n")); 
}


char *find_local_file_name(s)
char *s;
{
   tnd= f_header;
   while(tnd != NULL)
   {
     if(strcmp(tnd->P_name,s) == 0) return(tnd->l_name);
     tnd= tnd->next;
   }
   return(" "); 
}


put_under_line()
{
   int  i;

   for(i=1;i<80;i++)  write(fd,"-",strlen("-"));
   write(fd,"\n",strlen("\n"));
}

long gettime()		/* start timer */
{
	struct tms tbuf1;
	long tm; 

	tm = times(&tbuf1);
    	return(tbuf1.tms_stime);
}

/* Returns an e_loc name by (lkp). Otherwise, return get_a_cpu() */
char *find_host(lkp)
struct lk_list *lkp;
{
	struct lk_list *lk1;

	if (lkp == NULL) return get_a_cpu(0,0,0);
	if (lkp->curr_node->e_loc[0] == 0) 
		return get_a_cpu(0,0,0);
	return lkp->curr_node->e_loc;
}

void generate_slaves()
{
	struct node *t1;
	struct lk_list *lkpt;
	int idx;

/* Preprocessing  */
/* Removed to permit re-use of master processors. Yuan Shi 10/27/94 
	t1 = m_header;
	while (t1!=NULL)
	{
		if (t1->e_loc[0]!=0) {
			idx = find_host_idx(t1->e_loc);
			if (idx >= 0) ary_node_cap[idx].used++;
		}
		t1 = t1->next;
	}
*/
	/* check for redundant e_loc under the same TS object */
	t1 = f_header;
/*
	while (t1 != NULL)
	{
		if (t1->org == TS) {
			free_rcheck_list();
			lkpt = t1->succ_list;
			while (lkpt != NULL)
			{
				if (redundant(lkpt->curr_node->e_loc))
				{
	printf("++ CONF. Error: Redundant Slave location (%s) on (%s)\n",
					lkpt->curr_node->l_name,
					lkpt->curr_node->e_loc);
					exit(1);
				}
				lkpt = lkpt->next;
			}
		}
		t1 = t1->next;
	} 
*/
	t1 = m_header;
	while (t1!=NULL)
	{
		if (t1->org == SLAVE) generate_mdls(t1);
		t1 = t1->next;
	}
}

void generate_mdls(nodep)
struct node *nodep;
{
	struct node *tnew;
	struct lk_list *lkp, *lkp2;
	char name[MAX_STR_LEN],fsys[5], buf[128];
	int i, first=1, cluster_hosts = 0;

	if (nodep->e_loc[0] == 0) {
		strcpy(nodep->e_loc,get_a_cpu(1,0,0));
		if (isdigit(nodep->e_loc[0]))
			strcpy(buf,ary_node_cap[
				find_host_idx(nodep->e_loc)].name);
		else strcpy(buf,nodep->e_loc);
		printf("++      Automatic program assignment: (%s)->(%s)\n",
		    nodep->l_name, buf);
		if (nodep->e_path[0] == 0) strcpy(nodep->e_path, "*");
	}
	cluster_hosts = get_cluster_hosts(nodep->e_loc,fsys);
 
	for (i=0; i< cluster_hosts;i++)
	{
		if (first) first = 0;
		else {
			tnew = (struct node *)malloc(sizeof(struct node));
			sprintf(tnew->l_name,"%s%d",nodep->l_name,i);
			strcpy(tnew->loc,nodep->loc);
			if (nodep->P_name[0] == 0) 
			     strcpy(tnew->P_name,nodep->l_name);
			else strcpy(tnew->P_name,nodep->P_name);
			strcpy(tnew->e_P_name,nodep->e_P_name);
			strcpy(tnew->path, nodep->path);
			tnew->e_path[0] = 0;
			tnew->succ_list = nodep->succ_list;
			tnew->pred_list = nodep->pred_list;
			tnew->syn_list = nodep->syn_list;
			tnew->p_list = nodep->p_list;
			tnew->org = 'm';
			tnew->type = nodep->type;
			tnew->d = nodep->d;
			tnew->f = nodep->f;
			tnew->t = nodep->t;
			tnew->p = 0;
			strcpy(tnew->e_loc, get_a_cpu(1,1,fsys));
			printf("++      Automatic slave generation: (%s)->(%s)\n",
				tnew->l_name, tnew->e_loc);
			tnew->next = m_header;
			m_header = tnew;
			/* now repair the f_list */
			lkp = nodep->pred_list;
			while (lkp != NULL)
			{
				lkp2 = (struct lk_list *)malloc
					(sizeof(struct lk_list));
				lkp2->curr_node = tnew;
				lkp2->next = lkp->curr_node->succ_list;
				lkp->curr_node->succ_list = lkp2;
				lkp = lkp->next;
			}
			lkp = nodep->succ_list;
			while (lkp != NULL)
			{
				lkp2 = (struct lk_list *)malloc
					(sizeof(struct lk_list));
				lkp2->curr_node = tnew;
				lkp2->next = lkp->curr_node->pred_list;
				lkp->curr_node->pred_list = lkp2;
				lkp = lkp->next;
			}
		}
	}
}

int get_cluster_hosts(ipaddr, fsys)
char *ipaddr;
char *fsys;
{
	int i, cluster_cnt = 0;
	char buf[5];

	if (!isdigit(ipaddr[0])) strcpy(ipaddr,find_ip_addr(ipaddr));
	buf[0] = 0;
	for (i=0; i<= max_no_procs; i++)
	{
		if (!strcmp(ary_node_cap[i].ipaddr,ipaddr)) 
		{
			strcpy(buf, ary_node_cap[i].fsys);
			break;
		}
	}
	if (buf[0] == 0) return 0;
	for (i=0; i <= max_no_procs; i++)
	{
		if (!strcmp(ary_node_cap[i].fsys, buf)) cluster_cnt ++;
	}
	strcpy(fsys , buf);
	return cluster_cnt;
}

int redundant(name)
char *name;
{
	struct parm_list *t1;

	t1 = parm_header;
	while (t1 != NULL)
	{
		if (!strcmp(t1->parm,name)) return (1);
		t1 = t1->next;
	}
	t1 = (struct parm_list *) malloc(sizeof(t1));
	t1->parm = name;
	t1->next = parm_header;
	parm_header = t1;
	return (0);
}
	
void free_rcheck_list()
{
	struct parm_list *t1, *t2;
	
	t1 = parm_header;
	while (t1 != NULL)
	{
	 	t2 = t1;	
		t1 = t1->next;
		free(t2);		
	}
	parm_header = NULL;
}	

char *get_a_slave_cpu(lkpt)
struct lk_list *lkpt;
{
	struct lk_list *t1;

	if (global_slave_ptr == NULL) t1 = lkpt;
	else t1 = global_slave_ptr;
	while (t1 != NULL)
	{
		if (t1->curr_node->org == SLAVE)
		{
			global_slave_ptr = t1;
			return (t1->curr_node->e_loc);
		}
		t1 = t1->next;
	}
	return NULL;
}
	
