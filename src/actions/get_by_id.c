#include <microhttpd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libpq-fe.h>
#include <postgresql/server/catalog/pg_type_d.h>
#include <cjson/cJSON.h>
#include "actions.h"

FullResponse handle_get_by_id(PGconn *db_conn, const char *url)
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

    db_result = PQexecPrepared(db_conn, "get_monkey_by_id", 1, params, NULL, NULL, 0);

    int rows = PQntuples(db_result);
    int cols = PQnfields(db_result);

    printf("query returned %d rows and %d columns\n", rows, cols);

    if (rows == 0) {
        char *page = "monkey not found";

        response.response_content = MHD_create_response_from_buffer(strlen(page), page, MHD_RESPMEM_PERSISTENT);
        response.status_code = MHD_HTTP_NOT_FOUND;
    }
    else 
    {
        cJSON *result = cJSON_CreateObject();
        cJSON *monkey = cJSON_CreateObject();
        cJSON_AddItemToObject(result, "monkey", monkey);

        cJSON *id = cJSON_CreateNumber(atoi(PQgetvalue(db_result, 0, 0)));
        cJSON_AddItemToObject(monkey, "id", id);

        cJSON *name = cJSON_CreateString(PQgetvalue(db_result, 0, 1));
        cJSON_AddItemToObject(monkey, "name", name);

        cJSON *price = cJSON_CreateNumber(atoi(PQgetvalue(db_result, 0, 2)));
        cJSON_AddItemToObject(monkey, "price", price);

        cJSON *type = cJSON_CreateString(PQgetvalue(db_result, 0, 3));
        cJSON_AddItemToObject(monkey, "type", type);

        char *resultBuffer = cJSON_Print(result);
        
        response.response_content = MHD_create_response_from_buffer(strlen(resultBuffer), resultBuffer, MHD_RESPMEM_MUST_FREE);
        response.status_code = MHD_HTTP_OK;

        cJSON_Delete(result);
    }

    PQclear(db_result);

    return response;
}