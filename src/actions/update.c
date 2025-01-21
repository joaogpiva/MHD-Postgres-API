#include <postgres_ext.h>
#include <postgresql/server/catalog/pg_type_d.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <libpq-fe.h>
#include <cjson/cJSON.h>
#include <microhttpd.h>
#include "actions.h"

FullResponse handle_update(PostData *post_data, const char *url)
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
    const char *query_params[4];

    cJSON *payload = cJSON_Parse(post_data->data);

    cJSON *monkey = cJSON_GetObjectItem(payload, "monkey");

    if (monkey == NULL)
    {
        return invalid_parameter_error_response("monkey");
    }

    cJSON *name = cJSON_GetObjectItem(monkey, "name");

    if (name != NULL && (cJSON_IsString(name) && (name->valuestring != NULL)))
    {
        query_params[0] = name->valuestring;
    }
    else 
    {
        return invalid_parameter_error_response("name");
    }

    cJSON *price = cJSON_GetObjectItem(monkey, "price");

    if (price != NULL && cJSON_IsNumber(price))
    {
        char buffer[20];
        sprintf(buffer, "%d", price->valueint);
        query_params[1] = strdup(buffer);
    }
    else 
    {
        return invalid_parameter_error_response("price");
    }

    cJSON *type = cJSON_GetObjectItem(monkey, "type");

    if (type != NULL && (cJSON_IsString(type) && (type->valuestring != NULL)))
    {
        query_params[2] = type->valuestring;
    }
    else 
    {
        return invalid_parameter_error_response("type");
    }

    char buffer[20];
    sprintf(buffer, "%d", id);
    query_params[3] = strdup(buffer);

    Oid param_types[4] = {VARCHAROID, NUMERICOID, BPCHAROID, INT8OID}; 

    printf("updating id %s: %s, %s, %s\n", query_params[3], query_params[0], query_params[1], query_params[2]);

    db_conn = PQconnectdb(CONN_STR);

    PQsetSingleRowMode(db_conn);

    const char *query = "update monkeys set name = $1, price = $2, type = $3 where id = $4 returning id";

    db_result = PQexecParams(db_conn, query, 4, param_types, query_params, NULL, NULL, 0);

    char *updated_id = PQgetvalue(db_result, 0, 0);
    char *printed_return;

    if (!updated_id) {
        printf("%s", PQresultErrorMessage(db_result));
        printed_return = "could not find that id";
        response.status_code = MHD_HTTP_NOT_FOUND;
    } else {
        printed_return = "update successfull";
        response.status_code = MHD_HTTP_OK;
    }

    response.response_content = MHD_create_response_from_buffer(strlen(printed_return), printed_return, MHD_RESPMEM_PERSISTENT);

    cJSON_Delete(payload);
    PQclear(db_result);
    PQfinish(db_conn);

    return response;
}