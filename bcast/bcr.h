/*
 *             BCR.H ----------> BroadCast Reciever 
 *
 */

#include "synergy.h"

char                    bcid[BCKEY_LEN];
u_long                  lcid_hostid;
sng_int16               lcid_port;
int                     bcq_i =0, start_mailing =0;
cid_recieve_bc          in_bc, bc_q[BCQ_LEN];

void sigchldHandler ();
void mailman ();

