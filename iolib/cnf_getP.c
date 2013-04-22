/*---------------------------------------------------------------------
    cnf_getP() - returns the number of parallel workers. 
    RETURNS:  P value [1..N] integer. 
 ---------------------------------------------------------------------*/
#include "synergy.h"
#include "iolib.h"

int cnf_getP()
{
    if (sng_map_hd.d > 0) 
    printf("cnf_getP: (%d)\n",sng_map_hd.p);
	return(sng_map_hd.p);    
}
