#include <postgres_ext.h>
#include <postgresql/server/catalog/pg_type_d.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <libpq-fe.h>
#include <microhttpd.h>
#include "actions.h"

FullResponse handle_delete(PGconn *db_conn, const char *url)
{
    FullResponse response;

    int id = extract_id_from_url(url);

    if (id <= 0) {
        char *page = "couldn't find a valid ID in url, make sure your request is /monkey/{id} and id is greater than 0";

        response.response_content = MHD_create_response_from_buffer(strlen(page), page, MHD_RESPMEM_PERSISTENT);
        response.status_code = MHD_HTTP_BAD_REQUEST;

        return response;
    }

    PGresult *db_result;

    char id_str[12];
    snprintf(id_str, sizeof(id_str), "%d", id);

    const char *params[1] = { id_str };

    db_result = PQexecPrepared(db_conn, "delete_monkey", 1, params, NULL, NULL, 0);

    char *deleted_id = PQgetvalue(db_result, 0, 0);
    char *printed_return;

    if (!deleted_id) {
        printf("%s", PQresultErrorMessage(db_result));
        printed_return = "could not find that id";
        response.status_code = MHD_HTTP_NOT_FOUND;
    } else {
        printed_return = "delete successfull";
        response.status_code = MHD_HTTP_OK;
    }

    response.response_content = MHD_create_response_from_buffer(strlen(printed_return), printed_return, MHD_RESPMEM_MUST_FREE);

    PQclear(db_result);

    return response;
}