/*====================================================================*/
/*  Subroutine   cnf_dinit()                                          */
/*     Initializes the tid_list before each scatter operation.        */
/*                                                                    */
/*====================================================================*/
#include "synergy.h"
#include "iolib.h"

int  cnf_dinit( )
{
    tid_list *tid_ptr;
  
	tid_ptr = tid_header;
	while (tid_ptr != NULL)
	{
		tid_ptr->host = 0;
		tid_ptr->cid_port = 0;
		tid_ptr = tid_ptr->next;
	}
	return 1;  
}
