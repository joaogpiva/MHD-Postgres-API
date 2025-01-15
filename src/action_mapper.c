#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <regex.h>
#include "action_mapper.h"

const UrlActionMapping url_mappings[] = {
    { "/fusty", GET_IMAGE, 0 },
    { "/monkeys", GET_ALL, 0 },
    { "/monkey/(.+)", GET_BY_ID, 1 }
};

const int num_mappings = sizeof(url_mappings) / sizeof(url_mappings[0]);

enum Action get_action_from_url (const char *url)
{
    regex_t regex;

    for (int i = 0; i < num_mappings; i++)
    {
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