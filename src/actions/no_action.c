#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "actions.h"

FullResponse handle_no_action(const char *url, const char *method)
{
    FullResponse response;

    char *page;

    int size = snprintf(NULL, 0, "no %s route matched for %s", method, url) + 1;

    page = malloc(size);

    snprintf(page, size, "no %s route matched for %s", method, url);

    response.response_content = MHD_create_response_from_buffer(strlen(page), page, MHD_RESPMEM_PERSISTENT);
    response.status_code = MHD_HTTP_BAD_REQUEST;

    return response;
}