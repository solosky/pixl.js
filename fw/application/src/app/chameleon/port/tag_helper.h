#ifndef TAG_HELPER_H
#define TAG_HELPER_H

#include "tag_base_type.h"
#include "nfc_14a.h"
#include "nfc_mf1.h"
#include "nfc_ntag.h"

#define TAG_TYPE_MAX 9
#define TAG_TYPE_HF_MAX 8
#define SLOT_MAX 8

typedef struct {
    tag_specific_type_t tag_type;
    const char *short_name;
    const char *long_name;
    uint16_t data_size;
} tag_specific_type_name_t;

typedef enum {
    TAG_GROUP_UNKNOWN = 0,
    TAG_GROUP_MIFLARE,
    TAG_GROUP_NTAG,
} tag_group_type_t;

extern const tag_specific_type_t hf_tag_specific_types[];

const tag_specific_type_name_t* tag_helper_get_tag_type_name(tag_specific_type_t tag_type);
const nfc_tag_14a_coll_res_reference_t *tag_helper_get_active_coll_res_ref();
tag_specific_type_t tag_helper_get_active_tag_type();
void tag_helper_format_uid(char *buff, uint8_t *uid, uint8_t uid_len);
const char *tag_helper_get_mf_write_mode_name(nfc_tag_mf1_write_mode_t write_mode);

void tag_helper_get_nickname(char *buff, size_t buff_len);
int32_t tag_helper_set_nickname(const char *nickname);

size_t tag_helper_get_active_tag_data_size();
uint8_t *tag_helper_get_active_tag_memory_data();


#endif