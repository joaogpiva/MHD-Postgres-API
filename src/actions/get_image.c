#include <microhttpd.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include "actions.h"

#define FILENAME "./assets/fusty.png"
#define MIMETYPE "image/png"

FullResponse handle_get_image()
{
    int fd;
    struct stat sbuf;

    FullResponse response;
    
    if((fd = open(FILENAME, O_RDONLY)) == -1 ||
        fstat(fd, &sbuf) != 0)
    {
        printf("error reading file\n");

        if (fd != -1) close(fd);

        char *page = "error reading file";

        response.response_content = MHD_create_response_from_buffer(strlen(page), page, MHD_RESPMEM_PERSISTENT);
        response.status_code = MHD_HTTP_INTERNAL_SERVER_ERROR;
    }
    else
    {
        response.response_content = MHD_create_response_from_fd(sbuf.st_size, fd);
        response.status_code = MHD_HTTP_OK;
        MHD_add_response_header(response.response_content, MHD_HTTP_HEADER_CONTENT_TYPE, MIMETYPE);   
    }

    return response;
}