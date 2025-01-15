#include <stdio.h>
#include <regex.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <microhttpd.h>
#include <libpq-fe.h>
#include <postgresql/server/catalog/pg_type_d.h>
#include <cjson/cJSON.h>
#include "handler.h"
#include "action_mapper.h"
#include "actions.h"

enum MHD_Result answer_to_connection (
    void *cls, 
    struct MHD_Connection *connection,
    const char *url,
    const char *method, 
    const char *version,
    const char *upload_data,
    size_t *upload_data_size, 
    void **con_cls)
{
    log_request(url, method, version);

    FullResponse response;
    enum MHD_Result return_value = MHD_NO;

    enum Action action = get_action_from_url(url);

    switch (action) {
        case GET_IMAGE:
            response = handle_get_image();
            break;
        case GET_ALL:
            response = handle_get_all();
            break;
        case GET_BY_ID:
            response = handle_get_by_id(url);
            break;
        case CREATE:
        case UPDATE:
        case DELETE:
        case NO_ACTION:
            response = handle_no_action(url);
            break;
    }

    return_value = MHD_queue_response(connection, response.status_code, response.response_content);

    MHD_destroy_response(response.response_content);

    return return_value;
}

void log_request (const char *url, const char *method, const char *version)
{
    printf ("new %s request for %s using version %s\n", method, url, version);
}