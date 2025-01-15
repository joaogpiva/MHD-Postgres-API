#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <microhttpd.h>
#include "handler.h"
#include "action_mapper.h"

#define PORT 5000

int main()
{
    struct MHD_Daemon *daemon;

    daemon = MHD_start_daemon(MHD_USE_AUTO_INTERNAL_THREAD, PORT, NULL, NULL, &answer_to_connection, NULL, MHD_OPTION_END);

    if (daemon == NULL){
        printf("something went terribly wrong\n");
        return 1;
    }

    printf("listening on port %d\n", PORT);

    getchar();

    MHD_stop_daemon(daemon);

    printf("daemon stopped successfully\n");

    return 0;
}