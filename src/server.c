#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <microhttpd.h>
#include <libpq-fe.h>
#include "handler.h"
#include "action_mapper.h"
#include "database.h"

#define PORT 5000

PGconn *db_conn;

int main()
{
    struct MHD_Daemon *daemon;

    daemon = MHD_start_daemon(MHD_USE_AUTO_INTERNAL_THREAD, PORT, NULL, NULL, &answer_to_connection, NULL, MHD_OPTION_END);

    if (daemon == NULL){
        printf("something went terribly wrong\n");
        return EXIT_FAILURE;
    }

    db_conn = initialize_db();

    printf("listening on port %d\n", PORT);

    getchar();

    MHD_stop_daemon(daemon);
    PQfinish(db_conn);

    printf("daemon stopped successfully\n");

    return 0;
}