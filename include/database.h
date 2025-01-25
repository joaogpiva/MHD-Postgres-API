#include <libpq-fe.h>

#define CONN_STR "host=localhost port=5432 dbname=mhd_api_db user=mhd_api_user password=mhd_api_pw"

PGconn* initialize_db();
void prepare_statement(PGconn *conn, char *name, char *query, int args_amount);
void exit_with_error(PGconn *conn, char *operation);