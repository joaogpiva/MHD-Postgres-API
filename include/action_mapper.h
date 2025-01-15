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
    enum Action action;
    int is_regex;
} UrlActionMapping;

enum Action get_action_from_url (const char *url);