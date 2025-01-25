#pragma once

enum Action {
    GET_IMAGE,
    GET_ALL,
    GET_BY_ID,
    CREATE,
    UPDATE,
    DELETE,
    NO_ACTION
};

typedef struct {
    const char *pattern;
    const char *method;
    enum Action action;
    int is_regex;
} UrlActionMapping;

enum Action get_action_from_url_and_method (const char *url, const char *method);