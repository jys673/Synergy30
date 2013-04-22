/*---------------------------------------------------------------------
    cnf_getarg(idx) - returns the runtime argument by index. 
    RETURNS:  idx'th argument  (char *). 
 ---------------------------------------------------------------------*/
#include "synergy.h"
#include "iolib.h"

char *cnf_getarg(idx)
int idx;
{
	if ((idx < 0) || (idx >= ARGS_MAX))
	{
		printf("Bad arg index.\n");
		return 0;
	}
	if (sng_map_hd.d > 0) printf("cnf_getarg: (%s)\n",sng_map_hd.args[idx]);
	return(sng_map_hd.args[idx]);    
}
