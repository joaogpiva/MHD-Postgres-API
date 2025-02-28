#pragma once

#include <libpq-fe.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <microhttpd.h>

typedef struct 
{
    struct MHD_Response* response_content;
    int status_code;
} FullResponse;

typedef struct
{
    char *data;
    size_t length;
} PostData;

FullResponse handle_get_image();
FullResponse handle_get_all(PGconn *db_conn);
FullResponse handle_get_by_id(PGconn *db_conn, const char *url);
FullResponse handle_create(PGconn *db_conn, PostData *post_data);
FullResponse handle_update(PGconn *db_conn, PostData *post_data, const char *url);
FullResponse handle_delete(PGconn *db_conn, const char *url);
FullResponse handle_no_action(const char *url, const char *method);

static int extract_id_from_url(const char *url)
{
    char *token;
    int id = 0;
    char *url_copy = strdup(url);
    while ((token = strsep(&url_copy, "/"))) {
        if ((id = atoi(token))) {
            break;
        }
    }

    free(url_copy);

    return id;
}

static FullResponse invalid_parameter_error_response(char *param_name)
{
    FullResponse response;

    char *message;

    int size = snprintf(NULL, 0, "error parsing body, invalid parameter: %s", param_name) + 1;

    message = (char *)malloc(size);

    snprintf(message, size, "error parsing body, invalid parameter: %s", param_name);

    response.response_content = MHD_create_response_from_buffer(strlen(message), message, MHD_RESPMEM_PERSISTENT);
    response.status_code = MHD_HTTP_BAD_REQUEST;

    return response;
}