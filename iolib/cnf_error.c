/*----------------------------------------------------------------------------*/
/* This function prints to the user the kind of error encountered.            */
/* Its based on the error definitions in the file 
synergy.h               */
/*----------------------------------------------------------------------------*/
#include "../include/synergy.h"
int cnf_error( int errno )
{
    switch (errno) {
    case TSH_ER_NOERROR:
        printf("Normal operation - No error at all \n");
        break;
    case TSH_ER_INSTALL:
        printf("Error: Tuple Space daemon could not be started\n");
        break;
    case TSH_ER_NOTUPLE:
        printf("Error: Could not find such tuple \n");
        break;
    case TSH_ER_NOMEM:
        printf("Error: Tuple space daemon out of memory \n");
        break;
    case TSH_ER_OVERRT:
        printf("Warning: Tuple was overwritten \n");
        break;
    default:
        printf("Unknown Error %d \n", errno);
        break;
    }
    return(1);    
}
