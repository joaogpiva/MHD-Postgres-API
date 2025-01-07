#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <microhttpd.h>

#define PORT 8888

enum MHD_Result answer_to_connection (
    void *cls, struct MHD_Connection *connection,
    const char *url,
    const char *method, const char *version,
    const char *upload_data,
    size_t *upload_data_size, void **con_cls)
{
    const char *page = "<html><body>one of the webpages of all time</body></html>";

    struct MHD_Response *response;
    enum MHD_Result return_value;

    response = MHD_create_response_from_buffer(strlen(page), page, MHD_RESPMEM_PERSISTENT);

    return_value = MHD_queue_response(connection, MHD_HTTP_OK, response);

    MHD_destroy_response(response);

    return return_value;
}

int main(){
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