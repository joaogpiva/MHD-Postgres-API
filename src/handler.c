#include <stdio.h>
#include <regex.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <microhttpd.h>
#include <libpq-fe.h>
#include <postgresql/server/catalog/pg_type_d.h>
#include <cjson/cJSON.h>
#include <time.h>
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
    log_request(url, method, version, upload_data, upload_data_size);

    PostData *post_data;

    if(con_cls == NULL)
    {
        // con_cls tracks the state of the request, null means new request
        // "the MHD_AccessHandlerCallback will be invoked multiple times to process data as it arrives; at each invocation a new chunk of data must be processed"
        PostData *post_data = NULL;
    }
    else
    {
        // request is ongoing so just copy to post_data
        PostData *post_data = *con_cls;
    }

    if ((strcmp(method, "POST") == 0) || (strcmp(method, "PUT") == 0))
    {
        if (*con_cls == NULL) {
            printf("allocating memory for data...\n");
            PostData *post_data = malloc(sizeof(PostData));
            post_data->data = NULL;
            post_data->length = 0;
            *con_cls = post_data;
            return MHD_YES;
        }

        PostData *post_data = *con_cls;

        if (*upload_data_size > 0) {
            iterate_post(post_data, MHD_POSTDATA_KIND, NULL, NULL, NULL, NULL, upload_data, 0, *upload_data_size);
            *upload_data_size = 0; // upload_data and upload_data_size refer only to the current chunk so this can be set to 0 to "signal" that the data has been handled
            printf("still processing data...\n");
            return MHD_YES;
        }

        post_data = *con_cls;
        printf("finished processing data, content received: \n%s\n", post_data->data);
    }

    FullResponse response;
    enum MHD_Result return_value = MHD_NO;

    enum Action action = get_action_from_url_and_method(url, method);

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
            response = handle_create(post_data);
            break;
        case UPDATE:
            response = handle_update(post_data, url);
            break;
        case DELETE:
        case NO_ACTION:
            response = handle_no_action(url, method);
            break;
    }

    return_value = MHD_queue_response(connection, response.status_code, response.response_content);

    MHD_destroy_response(response.response_content);

    printf("\n");

    return return_value;
}

enum MHD_Result iterate_post(
    void *coninfo_cls,
    enum MHD_ValueKind kind,
    const char *key,
    const char *filename,
    const char *content_type, 
    const char *transfer_encoding,
    const char *data,
    uint64_t off,
    size_t size)
{
    PostData *post_data = coninfo_cls;

    if (size > 0) {
        // expand buffer to accommodate incoming data
        char *new_data = realloc(post_data->data, post_data->length + size + 1);
        if (!new_data) {
            fprintf(stderr, "Memory allocation error\n");
            return MHD_NO;
        }

        post_data->data = new_data;
        memcpy(post_data->data + post_data->length, data, size);
        post_data->length += size;
        post_data->data[post_data->length] = '\0';  // null-terminate
    }

    return MHD_YES;
}

void log_request (const char *url, const char *method, const char *version, const char *upload_data, const size_t *upload_data_size)
{
    printf("new %s request for %s using version %s\n", method, url, version);
    //printf("upload data received (size %ld): %s\n", *upload_data_size, upload_data);
    printf("\n");
}