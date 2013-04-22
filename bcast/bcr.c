/* This program (BCP.c) is forked by CID to receive bcast messages. It also
 * forks a child upon receipt of a bcast message.
 * Author: Feijian Sun
 * April 20, 1996
 */

#include        "bcr.h"

int main(argc, argv)
int     argc;
char   *argv[];
{
  struct sockaddr_in  serv_addr, cli_addr;
  int                 mm_pid, sockfd, lcliaddr;

  strcpy(bcid, argv[1]);
  sscanf(argv[2], "%ul\0", &lcid_hostid);
  lcid_port = atoi(argv[3]);
  /*
   * Open a UDP socket (an Internet datagram socket).
   */
  if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
{
printf("Error BCR::socket\n") ;
    exit(1);
}
  /*
   * Bind any local address for us.
   */
  bzero((char *) &serv_addr, sizeof(serv_addr));    /* zero out */
  serv_addr.sin_family      = AF_INET;
  serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  serv_addr.sin_port        = htons(SERV_UDP_PORT);

  if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
  {
printf("Error BCR::bind\n") ;
    close(sockfd);
    exit(1);
  }
  lcliaddr = sizeof(cli_addr);

  while (TRUE)
  {
    bzero((char *)&in_bc, sizeof(in_bc));
/*
printf("BCR::1::bcq_i:(%d)\n", bcq_i);
*/
    if (recvfrom(sockfd, (char *) &in_bc, sizeof(in_bc), 0, 
                         (struct sockaddr *) &cli_addr, &lcliaddr) < 0)
    {
printf("Error BCR::recvfrom\n") ;
      continue;
/*
      close(sockfd);
      exit(1);
*/
    }
    if (!strcmp(in_bc.bckey, bcid) && (in_bc.host != lcid_hostid) &&
				      (in_bc.port != lcid_port))
    {
      bcopy((char *)&in_bc, (char *)&bc_q[bcq_i], sizeof(in_bc));
      if (++bcq_i == BCQ_LEN) 
      { 
printf("Error BCR::BC queue\n") ;
        fprintf(stderr, "BCR::BC queue\n") ;
        close(sockfd);
        exit(1) ;
      }
/*
printf("BCR::2::bcq_i:(%d)\n", bcq_i);
*/
      if (start_mailing == 0)
      {
	start_mailing= 1;
        if ((mm_pid = fork()) == 0 )
        {
          mailman();
printf("Error BCR::mailman\n") ;
	  close(sockfd);
          exit(-1);
        }
	bcq_i        = 0;
        if (signal(SIGCHLD, sigchldHandler) == (void *)-1)
        {
printf("Error BCR::signal\n") ;
          fprintf(stderr, "BCR::signal\n") ;
	  close(sockfd);
          exit(1) ;
        }
      }
    }
  }
  close(sockfd);
printf("Error BCR::exit\n") ;
  exit(0);
}


/*---------------------------------------------------------------------------
  Prototype   : void sigchldHandler(void)
  Parameters  : -
  Returns     : -
  Called by   : BCR 
  Calls       :
  Notes       : 
  Date        : 
  Coded by    :
  Modification: 
---------------------------------------------------------------------------*/

void sigchldHandler()
{
  int mm_pid, status;

  mm_pid = wait(&status) ;
  if (bcq_i > 0)
  {
/*
printf("BCR (in sigchldHandler)::1::bcq_i:(%d)\n", bcq_i) ;
*/
    start_mailing= 1;
    if ((mm_pid = fork()) == 0 )
    {
printf("Error BCR (in sigchldHandler)::mailman\n") ;
      mailman();
      exit(-1);
    }
    bcq_i        = 0;
    if (signal(SIGCHLD, sigchldHandler) == (void *)-1)
    {
printf("Error BCR (in sigchldHandler)::signal\n") ;
      fprintf(stderr, "BCR::signal\n") ;
      exit(1) ;
    }
  }
  else
{
/*
printf("BCR (in sigchldHandler)::2::bcq_i:(%d)\n", bcq_i) ;
*/
    start_mailing= 0;
}
}


/*---------------------------------------------------------------------------
  Prototype   : void mailman(void)
  Parameters  : -
  Returns     : -
  Called by   : BCR
  Calls       :
  Notes       :
  Date        :
  Coded by    :
  Modification:
---------------------------------------------------------------------------*/

void mailman()
{
  u_short        this_op;
  int            sock;

/*
printf("BCR (in mailman):: mailman start\n") ;
*/
  this_op   = CID_OP_VT_UPD;
  if ((sock = get_socket()) != -1)
    if (do_connect(sock, lcid_hostid, lcid_port))
      if (writen(sock, (char *)&this_op, sizeof(u_short)))
        if (writen(sock, (char *)&bcq_i, sizeof(int)))
          if (writen(sock, (char *)&bc_q, BCQ_LEN))
            ;
  close(sock);
/*
printf("BCR (in mailman):: mailman exit\n") ;
*/
  exit(0);
}

