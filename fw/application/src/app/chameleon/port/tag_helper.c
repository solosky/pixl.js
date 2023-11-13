#include "tag_helper.h"
#include "fds_ids.h"
#include "fds_utils.h"
#include <string.h>

const static tag_specific_type_name_t tag_type_names[] = {
    [TAG_TYPE_UNKNOWN] = {"UKNOWN", "UKNOWN"},
    // 125kHz (ID card) series
    [TAG_TYPE_EM410X] = {"EM410X", "EM410X"},
    // MiFare series
    [TAG_TYPE_MIFARE_Mini] = {"MF Mini", "MiFare Mini"},
    [TAG_TYPE_MIFARE_1024] = {"MF 1K", "MiFare 1K"},
    [TAG_TYPE_MIFARE_2048] = {"MF 2K", "MiFare 2K"},
    [TAG_TYPE_MIFARE_4096] = {"MF 4K", "MiFare 4K"},
    // NTAG series
    [TAG_TYPE_NTAG_213] = {"N213", "NTAG 213"},
    [TAG_TYPE_NTAG_215] = {"N215", "NTAG 215"},
    [TAG_TYPE_NTAG_216] = {"N216", "NTAG 216"},
};

// typedef enum {
//     NFC_TAG_MF1_WRITE_NORMAL    = 0u,
//     NFC_TAG_MF1_WRITE_DENIED    = 1u,
//     NFC_TAG_MF1_WRITE_DECEIVE   = 2u,
//     NFC_TAG_MF1_WRITE_SHADOW    = 3u,
// } nfc_tag_mf1_write_mode_t;
const static char *mf1_write_mode_names[] = {
    [NFC_TAG_MF1_WRITE_NORMAL] = "正常",
    [NFC_TAG_MF1_WRITE_DENIED] = "拒绝",
    [NFC_TAG_MF1_WRITE_DECEIVE] = "忽略",
    [NFC_TAG_MF1_WRITE_SHADOW] = "缓存",
};

const tag_specific_type_t hf_tag_specific_types[] = {
    // Specific and necessary signs do not exist
    TAG_TYPE_UNKNOWN,
    // MiFare series
    TAG_TYPE_MIFARE_Mini,
    TAG_TYPE_MIFARE_1024,
    TAG_TYPE_MIFARE_2048,
    TAG_TYPE_MIFARE_4096,
    // NTAG series
    TAG_TYPE_NTAG_213,
    TAG_TYPE_NTAG_215,
    TAG_TYPE_NTAG_216,
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

const nfc_tag_14a_coll_res_reference_t *tag_helper_get_active_coll_res_ref() {
    tag_specific_type_t tag_type = tag_helper_get_active_tag_type();
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

tag_specific_type_t tag_helper_get_active_tag_type() {
    tag_specific_type_t tag_type[2];
    uint8_t slot = tag_emulation_get_slot();
    tag_emulation_get_specific_type_by_slot(slot, tag_type);
    return tag_type[0];
}

void tag_helper_format_uid(char *buff, uint8_t *uid, uint8_t uid_len) {
    if (uid_len == NFC_TAG_14A_UID_SINGLE_SIZE) {
        sprintf(buff, "%02x:%02x:%02x:%02x", uid[0], uid[1], uid[2], uid[3]);
    } else if (uid_len == NFC_TAG_14A_UID_DOUBLE_SIZE) {
        sprintf(buff, "%02x:%02x:%02x:%02x:%02x:%02x:%02x", uid[0], uid[1], uid[2], uid[3], uid[4], uid[5], uid[6]);
    } else {
        sprintf(buff, "%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x", uid[0], uid[1], uid[2], uid[3], uid[4],
                uid[5], uid[6], uid[7], uid[8], uid[9]);
    }
}

const char *tag_helper_get_mf_write_mode_name(nfc_tag_mf1_write_mode_t write_mode) {
    return mf1_write_mode_names[write_mode];
}

void tag_helper_get_nickname(char *buff, size_t buff_len) {
    uint8_t slot = tag_emulation_get_slot();
    vfs_meta_t meta = {0};
    memset(buff, 0, buff_len);
    if (fds_read_meta(FDS_SLOT_TAG_DUMP_FILE_ID_BASE + slot, TAG_SENSE_HF, &meta)) {
        if (meta.has_notes) {
            strncpy(buff, meta.notes, buff_len);
        }
    }

    if (strlen(buff) == 0) {
        sprintf(buff, "卡槽 %02d", slot + 1);
    }
}