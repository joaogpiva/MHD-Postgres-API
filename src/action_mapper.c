#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <regex.h>
#include "action_mapper.h"

const UrlActionMapping url_mappings[] = {
    { "/fusty", "GET", GET_IMAGE, 0 },
    { "/monkeys", "GET", GET_ALL, 0 },
    { "/monkey/(.+)", "GET", GET_BY_ID, 1 },
    { "/monkey(/?)", "POST", CREATE, 1}
};

const int num_mappings = sizeof(url_mappings) / sizeof(url_mappings[0]);

enum Action get_action_from_url_and_method (const char *url, const char *method)
{
    regex_t regex;

    for (int i = 0; i < num_mappings; i++)
    {
        if (strcmp(method, url_mappings[i].method) != 0) {
            continue;
        }

        if (url_mappings[i].is_regex) {
            regcomp(&regex, url_mappings[i].pattern, REG_EXTENDED);

            if (regexec(&regex, url, 0, NULL, 0) == 0) {
                regfree(&regex);
                return url_mappings[i].action;
            }
            regfree(&regex);
        } else {
            if (strcmp(url, url_mappings[i].pattern) == 0) {
                return url_mappings[i].action;
            }
        }
    }

    return NO_ACTION;
}