#include <microhttpd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libpq-fe.h>
#include <postgresql/server/catalog/pg_type_d.h>
#include <cjson/cJSON.h>
#include "actions.h"

FullResponse handle_get_all(PGconn *db_conn)
{
    PGresult *db_result;

    FullResponse response;

    db_result = PQexecPrepared(db_conn, "get_all_monkeys", 0, NULL, NULL, NULL, 0);

    int rows = PQntuples(db_result);
    int cols = PQnfields(db_result);

    printf("query returned %d rows and %d columns\n", rows, cols);

    cJSON *result = cJSON_CreateObject();
    cJSON *monkeys = cJSON_CreateArray();
    cJSON_AddItemToObject(result, "monkeys", monkeys);

    for (int i = 0; i < rows; i++) {
        cJSON *parsedRow = cJSON_CreateObject();

        cJSON *id = cJSON_CreateNumber(atoi(PQgetvalue(db_result, i, 0)));
        cJSON_AddItemToObject(parsedRow, "type", id);

        cJSON *name = cJSON_CreateString(PQgetvalue(db_result, i, 1));
        cJSON_AddItemToObject(parsedRow, "name", name);

        cJSON *price = cJSON_CreateNumber(atoi(PQgetvalue(db_result, i, 2)));
        cJSON_AddItemToObject(parsedRow, "price", price);

        cJSON *type = cJSON_CreateString(PQgetvalue(db_result, i, 3));
        cJSON_AddItemToObject(parsedRow, "type", type);

        cJSON_AddItemToArray(monkeys, parsedRow);
    }

    char *resultBuffer = cJSON_Print(result);

    response.response_content = MHD_create_response_from_buffer(strlen(resultBuffer), resultBuffer, MHD_RESPMEM_MUST_FREE);
    response.status_code = MHD_HTTP_OK;

    cJSON_Delete(result);

    PQclear(db_result);

    return response;
}