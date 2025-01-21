#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libpq-fe.h>
#include <postgresql/server/catalog/pg_type_d.h>
#include <cjson/cJSON.h>
#include "actions.h"

FullResponse handle_get_by_id(const char *url)
{
    FullResponse response;

    int id = extract_id_from_url(url);

    if (id <= 0) {
        char *page = "couldn't find a valid ID in url, make sure your request is /monkey/{id} and id is greater than 0";

        response.response_content = MHD_create_response_from_buffer(strlen(page), page, MHD_RESPMEM_PERSISTENT);
        response.status_code = MHD_HTTP_BAD_REQUEST;

        return response;
    }

    PGconn *db_conn;
    PGresult *db_result;

    db_conn = PQconnectdb(CONN_STR);

    char *query;

    int size = snprintf(NULL, 0, "SELECT * FROM monkeys WHERE id = %d", id) + 1;

    query = malloc(size);

    snprintf(query, size, "SELECT * FROM monkeys WHERE id = %d", id);

    db_result = PQexec(db_conn, query);

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

        for (int j = 0; j < cols; j++) {
            switch (PQftype(db_result, j)) {
                case VARCHAROID:
                case BPCHAROID:
                    cJSON *parsedText = cJSON_CreateString(PQgetvalue(db_result, 0, j));
                    cJSON_AddItemToObject(monkey, PQfname(db_result, j), parsedText);
                    break;
                case INT2OID:
                case INT4OID:
                case INT8OID:
                    cJSON *parsedInteger = cJSON_CreateNumber(atoi(PQgetvalue(db_result, 0, j)));
                    cJSON_AddItemToObject(monkey, PQfname(db_result, j), parsedInteger);
                    break;
            }
        }

        char *resultBuffer = cJSON_Print(result);
        
        response.response_content = MHD_create_response_from_buffer(strlen(resultBuffer), resultBuffer, MHD_RESPMEM_PERSISTENT);
        response.status_code = MHD_HTTP_OK;

        cJSON_Delete(result);
    }

    free(query);

    PQclear(db_result);
    PQfinish(db_conn);

    return response;
}