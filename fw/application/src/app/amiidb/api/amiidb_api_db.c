//
// Created by solos on 8/20/2023.
//

#include "amiidb_api_db.h"
#include <string.h>

extern char *strcasestr(const char *, const char *);

int32_t amiidb_api_db_search(const char *search, amiidb_db_search_cb_t cb, void *ctx) {
    const db_amiibo_t *p_amiibo = amiibo_list;
    while (p_amiibo->name_en != 0) {
        if (strlen(search) > 0 && strcasestr(p_amiibo->name_en, search) != NULL) {
            cb(p_amiibo, ctx);
        }
        p_amiibo++;
    }
    return 0;
}