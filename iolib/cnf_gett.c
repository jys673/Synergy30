/*---------------------------------------------------------------------
    cnf_gett() - returns the threshold value for loop scheduling.
    RETURNS:  t value [1..N) integer. 
 ---------------------------------------------------------------------*/
#include "synergy.h"
#include "iolib.h"

int cnf_gett()
{
    if (sng_map_hd.d > 0) 
    printf("cnf_gett: (%d)\n",sng_map_hd.t);
	return(sng_map_hd.t);    
}
