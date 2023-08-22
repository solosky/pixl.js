//
// Created by solos on 8/20/2023.
//

#ifndef FW_AMIIDB_API_DB_H
#define FW_AMIIDB_API_DB_H

#include "db_header.h"

typedef void (*amiidb_db_search_cb_t)(db_amiibo_t *p_amiibo, void *ctx);

int32_t amiidb_api_db_search(const char *search, amiidb_db_search_cb_t cb, void *ctx);

#endif // FW_AMIIDB_API_DB_H
