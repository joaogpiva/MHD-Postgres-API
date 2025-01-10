#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <microhttpd.h>
#include <libpq-fe.h>
#include <postgresql/server/catalog/pg_type_d.h>
#include <cjson/cJSON.h>

#define PORT 8888
#define FILENAME "assets/fusty.png"
#define MIMETYPE "image/png"
#define CONN_STR "host=localhost port=5432 dbname=mhd_api_db user=mhd_api_user password=mhd_api_pw"

enum MHD_Result print_out_key (void *cls, enum MHD_ValueKind kind, const char *key, const char *value);

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
    printf ("New %s request for %s using version %s\n", method, url, version);

    struct MHD_Response *response;
    enum MHD_Result return_value;

    if (strcmp(method, "GET") != 0) {
        response = MHD_create_response_empty(MHD_RF_NONE);

        return_value = MHD_queue_response(connection, MHD_HTTP_NOT_ACCEPTABLE, response);
    }

    if (strcmp(url, "/fusty") == 0) {
        int fd;
        struct stat sbuf;
        
        if((fd = open(FILENAME, O_RDONLY)) == -1 ||
            fstat(fd, &sbuf) != 0)
        {
            printf("error reading file\n");

            if (fd != -1) close(fd); // close fd in case fd worked but fstat didnt for some reason

            const char *page = "error reading file";

            response = MHD_create_response_from_buffer(strlen(page), page, MHD_RESPMEM_PERSISTENT);

            return_value = MHD_queue_response(connection, MHD_HTTP_INTERNAL_SERVER_ERROR, response);
        }
        else
        {
            response = MHD_create_response_from_fd(sbuf.st_size, fd);
            MHD_add_response_header(response, MHD_HTTP_HEADER_CONTENT_TYPE, MIMETYPE);

            return_value = MHD_queue_response(connection, MHD_HTTP_OK, response);
        }
    }
    else if (strcmp(url, "/monkeys") == 0){
        PGconn *conn;
        PGresult *res;

        conn = PQconnectdb(CONN_STR);

        res = PQexec(conn, "SELECT * FROM monkeys");

        int rows = PQntuples(res);
        int cols = PQnfields(res);

        printf("query returned %d rows and %d columns\n", rows, cols);

        cJSON *result = cJSON_CreateObject();
        cJSON *monkeys = cJSON_CreateArray();
        cJSON_AddItemToObject(result, "monkeys", monkeys);

        for (int i = 0; i < rows; i++) {
            cJSON *parsedRow = cJSON_CreateObject();

            for (int j = 0; j < cols; j++) {
                switch (PQftype(res, j)) {
                    case VARCHAROID:
                    case BPCHAROID:
                        cJSON *parsedText = cJSON_CreateString(PQgetvalue(res, i, j));
                        cJSON_AddItemToObject(parsedRow, PQfname(res, j), parsedText);
                        break;
                    case INT2OID:
                    case INT4OID:
                    case INT8OID:
                        cJSON *parsedInteger = cJSON_CreateNumber(atoi(PQgetvalue(res, i, j)));
                        cJSON_AddItemToObject(parsedRow, PQfname(res, j), parsedInteger);
                        break;
                }
            }

            cJSON_AddItemToArray(monkeys, parsedRow);
        }

        char *resultBuffer = cJSON_Print(result);
        response = MHD_create_response_from_buffer(strlen(resultBuffer), resultBuffer, MHD_RESPMEM_PERSISTENT);

        return_value = MHD_queue_response(connection, MHD_HTTP_OK, response);

        PQclear(res);
        cJSON_Delete(result);
    }

    MHD_destroy_response(response);

    return return_value;
}

int main(){
    struct MHD_Daemon *daemon;

    daemon = MHD_start_daemon(MHD_USE_AUTO_INTERNAL_THREAD, PORT, NULL, NULL, &answer_to_connection, NULL, MHD_OPTION_END);

    if (daemon == NULL){
        printf("something went terribly wrong\n");
        return 1;
    }

    printf("listening on port %d\n", PORT);

    getchar();

    MHD_stop_daemon(daemon);

    printf("daemon stopped successfully\n");

    return 0;
}

enum MHD_Result print_out_key (
    void *cls, 
    enum MHD_ValueKind kind,
    const char *key, 
    const char *value)
{
    printf ("%s: %s\n", key, value);
    return MHD_YES;
}