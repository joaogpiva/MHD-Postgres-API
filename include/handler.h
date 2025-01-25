#pragma once

#include <microhttpd.h>

enum MHD_Result iterate_post(
    void *coninfo_cls,
    enum MHD_ValueKind kind,
    const char *key,
    const char *filename,
    const char *content_type, 
    const char *transfer_encoding,
    const char *data,
    uint64_t off,
    size_t size);

enum MHD_Result answer_to_connection (
    void *cls, 
    struct MHD_Connection *connection,
    const char *url,
    const char *method, 
    const char *version,
    const char *upload_data,
    size_t *upload_data_size, 
    void **con_cls);

void log_request (
    const char *url,
    const char *method,
    const char *version,
    const char *upload_data,
    const size_t *upload_data_size);