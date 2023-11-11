#include "tag_helper.h"
#include "nfc_mf1.h"
#include "nfc_ntag.h"

const static tag_specific_type_name_t tag_type_names[] = {
    [TAG_TYPE_UNKNOWN] = {"UKNOWN", "UKNOWN"},
    // 125kHz (ID card) series
    [TAG_TYPE_EM410X] = {"EM410X", "EM410X"},
    // MiFare series
    [TAG_TYPE_MIFARE_Mini] = {"MF Mini", "MiFare Mini"},
    [TAG_TYPE_MIFARE_1024] = {"MF 1K", "MiFare 1K"},
    [TAG_TYPE_MIFARE_2048] = {"MF 2K", "MiFare 4K"},
    [TAG_TYPE_MIFARE_4096] = {"MF 2K", "MiFare 4K"},
    // NTAG series
    [TAG_TYPE_NTAG_213] = {"N213", "NTAG 213"},
    [TAG_TYPE_NTAG_215] = {"N215", "NTAG 215"},
    [TAG_TYPE_NTAG_216] = {"N216", "NTAG 216"},
};

tag_group_type_t tag_helper_get_tag_group_type(tag_specific_type_t tag_type) {
    if (tag_type == TAG_TYPE_MIFARE_Mini || tag_type == TAG_TYPE_MIFARE_1024 || tag_type == TAG_TYPE_MIFARE_2048 ||
        tag_type == TAG_TYPE_MIFARE_4096) {
        return TAG_GROUP_MIFLARE;
    } else if (tag_type == TAG_TYPE_NTAG_213 || tag_type == TAG_TYPE_NTAG_215 || tag_type == TAG_TYPE_NTAG_216) {
        return TAG_GROUP_NTAG;
    } else {
        return TAG_GROUP_UNKNOWN;
    }
}

const tag_specific_type_name_t *tag_helper_get_tag_type_name(tag_specific_type_t tag_type) {
    if (tag_type >= 0 && tag_type < TAG_TYPE_MAX) {
        return &tag_type_names[tag_type];
    } else {
        return 0;
    }
}

const nfc_tag_14a_coll_res_reference_t *tag_helper_get_tag_type_coll_res_entity(tag_specific_type_t tag_type) {
    tag_data_buffer_t *tag_buffer = get_buffer_by_tag_type(tag_type);
    tag_group_type_t tag_group_type = tag_helper_get_tag_group_type(tag_type);
    if (tag_group_type == TAG_GROUP_MIFLARE) {
        return get_mifare_coll_res();
        // nfc_tag_mf1_information_t *m_tag_information = (nfc_tag_mf1_information_t *)tag_buffer->buffer;
        // return &m_tag_information->res_coll;
    } else {
        // nfc_tag_ntag_information_t *m_tag_information = (nfc_tag_ntag_information_t *)tag_buffer->buffer;
        // return &m_tag_information->res_coll;
        return get_ntag_coll_res();
    }
}