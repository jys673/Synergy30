#include "synergy.h"

int _distributor;
int _constructor;

char _tp_token[255];
int _tokens = 0;

int _status = 0;
char _tp_name[TUPLENAME_LEN];
int _tp_size = 0;

int _activate_space(int id, char *workerId);
int _cleanup_queue(int id, char *workerId);
int _cleanup_space(int id, char *workerId);

int _open_space(char *local_name, char *mode, char *workerId);
int _close_space(int id, char *workerId, int cleanup);

int _send_data(int id, char *tpname, char *tpvalue, int tpsize);
int _read_data(int id, char *tpname, char *tpvalue, int tpsize);

int _set_token(int id, char *tpname, char *tpvalue, int tpsize);
int _get_token(int id, char *tpname, char *tpvalue, int tpsize);

int _init_space();

