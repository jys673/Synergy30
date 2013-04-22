/*---------------------------------------------------------------------
    cnf_term() - called before image return to clean things up.  closes
    any files left open.
    RETURNS:  nothing.
 ---------------------------------------------------------------------*/
#include "synergy.h"
#include "iolib.h"

void cnf_term()
{
    int id;

    if (sng_map_hd.d > 0) 
    printf("cnf_term: terminating ... \n");
    
    /* DAC will close all objects */
//    for (id=0; id<sng_idx; id++)
//	if (handles[id]->open)  cnf_close(id);
    exit(NORMAL_EXIT);
}
