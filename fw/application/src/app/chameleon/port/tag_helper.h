#ifndef TAG_HELPER_H
#define TAG_HELPER_H

#include "tag_base_type.h"
#include "nfc_14a.h"

#define TAG_TYPE_MAX 9

typedef struct {
    const char *short_name;
    const char *long_name;
} tag_specific_type_name_t;

typedef enum {
    TAG_GROUP_UNKNOWN = 0,
    TAG_GROUP_MIFLARE,
    TAG_GROUP_NTAG,
} tag_group_type_t;


const tag_specific_type_name_t* tag_helper_get_tag_type_name(tag_specific_type_t tag_type);
const nfc_tag_14a_coll_res_reference_t* tag_helper_get_tag_type_coll_res_entity(tag_specific_type_t tag_type);


#endif