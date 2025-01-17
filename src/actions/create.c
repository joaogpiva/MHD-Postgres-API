#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <libpq-fe.h>
#include <cjson/cJSON.h>
#include <microhttpd.h>
#include "actions.h"

FullResponse invalid_parameter_error_response(char *param_name);

FullResponse handle_create(PostData *post_data)
{
    FullResponse response;

    PGconn *db_conn;
    PGresult *db_result;
    const char *query_params[3];

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
        sprintf(query_params[1], "%d", price->valueint);
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

    printf("creating: %s, %s, %s\n", query_params[0], query_params[1], query_params[2]);

    db_conn = PQconnectdb(CONN_STR);

    PQsetSingleRowMode(db_conn);

    const char *query = "insert into monkeys(name, price, type) values ($1, $2, $3) returning id";

    db_result = PQexecParams(db_conn, query, 3, NULL, query_params, NULL, NULL, 0);

    char *id = PQgetvalue(db_result, 0, 0);
    printf("inserted row id: %s\n", id);

    PQclear(db_result);
    PQfinish(db_conn);

    cJSON *content = cJSON_CreateObject();
    cJSON *return_monkey = cJSON_CreateObject();

    cJSON_AddItemToObject(content, "monkey", return_monkey);

    cJSON *inserted_id = cJSON_CreateNumber(atoi(id));
    cJSON_AddItemToObject(return_monkey, "id", inserted_id);

    cJSON *inserted_name = cJSON_CreateString(query_params[0]);
    cJSON_AddItemToObject(return_monkey, "name", inserted_name);

    cJSON *inserted_price = cJSON_CreateNumber(atoi(query_params[1]));
    cJSON_AddItemToObject(return_monkey, "price", inserted_price);

    cJSON *inserted_type = cJSON_CreateString(query_params[2]);
    cJSON_AddItemToObject(return_monkey, "type", inserted_type);

    char *printed_return = cJSON_Print(content);

    response.response_content = MHD_create_response_from_buffer(strlen(printed_return), printed_return, MHD_RESPMEM_PERSISTENT);
    response.status_code = MHD_HTTP_OK;

    cJSON_Delete(payload);
    cJSON_Delete(content);

    return response;
}

FullResponse invalid_parameter_error_response(char *param_name)
{
    FullResponse response;

    char *message;

    int size = snprintf(NULL, 0, "error parsing body, invalid parameter: %s", param_name) + 1;

    message = malloc(size);

    snprintf(message, size, "error parsing body, invalid parameter: %s", param_name);

    response.response_content = MHD_create_response_from_buffer(strlen(message), message, MHD_RESPMEM_PERSISTENT);
    response.status_code = MHD_HTTP_BAD_REQUEST;

    return response;
}