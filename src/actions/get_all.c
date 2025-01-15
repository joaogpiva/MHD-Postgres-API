#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libpq-fe.h>
#include <postgresql/server/catalog/pg_type_d.h>
#include <cjson/cJSON.h>
#include "actions.h"

FullResponse handle_get_all()
{
    PGconn *db_conn;
    PGresult *db_result;

    FullResponse response;

    db_conn = PQconnectdb(CONN_STR);

    db_result = PQexec(db_conn, "SELECT * FROM monkeys");

    int rows = PQntuples(db_result);
    int cols = PQnfields(db_result);

    printf("query returned %d rows and %d columns\n", rows, cols);

    cJSON *result = cJSON_CreateObject();
    cJSON *monkeys = cJSON_CreateArray();
    cJSON_AddItemToObject(result, "monkeys", monkeys);

    for (int i = 0; i < rows; i++) {
        cJSON *parsedRow = cJSON_CreateObject();

        for (int j = 0; j < cols; j++) {
            switch (PQftype(db_result, j)) {
                case VARCHAROID:
                case BPCHAROID: 
                    cJSON *parsedText = cJSON_CreateString(PQgetvalue(db_result, i, j));
                    cJSON_AddItemToObject(parsedRow, PQfname(db_result, j), parsedText);
                    break;
                case INT2OID:
                case INT4OID:
                case INT8OID:
                    cJSON *parsedInteger = cJSON_CreateNumber(atoi(PQgetvalue(db_result, i, j)));
                    cJSON_AddItemToObject(parsedRow, PQfname(db_result, j), parsedInteger);
                    break;
            }
        }

        cJSON_AddItemToArray(monkeys, parsedRow);
    }

    char *resultBuffer = cJSON_Print(result);

    response.response_content = MHD_create_response_from_buffer(strlen(resultBuffer), resultBuffer, MHD_RESPMEM_PERSISTENT);
    response.status_code = MHD_HTTP_OK;

    cJSON_Delete(result);

    PQclear(db_result);
    PQfinish(db_conn);

    return response;
}