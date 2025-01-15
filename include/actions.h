#include <microhttpd.h>

#define CONN_STR "host=localhost port=5432 dbname=mhd_api_db user=mhd_api_user password=mhd_api_pw"

typedef struct 
{
    struct MHD_Response* response_content;
    int status_code;
} FullResponse;

FullResponse handle_get_image();
FullResponse handle_get_all();
FullResponse handle_get_by_id(const char *url);
FullResponse handle_no_action(const char *url);