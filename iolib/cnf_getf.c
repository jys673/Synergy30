/*---------------------------------------------------------------------
    cnf_getf() - returns the factor value for loop scheduling.
    RETURNS:  f value (0..100] integer. 
 ---------------------------------------------------------------------*/
#include "synergy.h"
#include "iolib.h"

int cnf_getf()
{
	if (sng_map_hd.d > 0) 
	printf("cnf_getf: (%d)\n",sng_map_hd.f);
	return(sng_map_hd.f);    
}
