#include "tag_helper.h"
#include "fds_ids.h"
#include "fds_utils.h"
#include "i18n/language.h"
#include "nrf_log.h"
#include "utils2.h"
#include <string.h>

const static tag_specific_type_name_t tag_type_names[] = {
    {TAG_TYPE_UNDEFINED, "-", "-", 0},
    // MiFare series
    {TAG_TYPE_MIFARE_Mini, "MF Mini", "MiFare Mini", 20 * NFC_TAG_MF1_DATA_SIZE},
    {TAG_TYPE_MIFARE_1024, "MF 1K", "MiFare 1K", 64 * NFC_TAG_MF1_DATA_SIZE},
    {TAG_TYPE_MIFARE_2048, "MF 2K", "MiFare 2K", 128 * NFC_TAG_MF1_DATA_SIZE},
    {TAG_TYPE_MIFARE_4096, "MF 4K", "MiFare 4K", 256 * NFC_TAG_MF1_DATA_SIZE},
    // NTAG series
    {TAG_TYPE_NTAG_213, "N213", "NTAG 213", 45 * NFC_TAG_NTAG_DATA_SIZE},
    {TAG_TYPE_NTAG_215, "N215", "NTAG 215", 135 * NFC_TAG_NTAG_DATA_SIZE},
    {TAG_TYPE_NTAG_216, "N216", "NTAG 216", 231 * NFC_TAG_NTAG_DATA_SIZE},
};
// typedef enum {
//     NFC_TAG_MF1_WRITE_NORMAL    = 0u,
//     NFC_TAG_MF1_WRITE_DENIED    = 1u,
//     NFC_TAG_MF1_WRITE_DECEIVE   = 2u,
//     NFC_TAG_MF1_WRITE_SHADOW    = 3u,
// } nfc_tag_mf1_write_mode_t;

const static uint32_t mf1_write_mode_names[4] = {
    [NFC_TAG_MF1_WRITE_NORMAL] = _L_APP_CHAMELEON_TAG_MF1_WRITE_NORMAL,
    [NFC_TAG_MF1_WRITE_DENIED] = _L_APP_CHAMELEON_TAG_MF1_WRITE_DENIED,
    [NFC_TAG_MF1_WRITE_DECEIVE] = _L_APP_CHAMELEON_TAG_MF1_WRITE_DECEIVE,
    [NFC_TAG_MF1_WRITE_SHADOW] = _L_APP_CHAMELEON_TAG_MF1_WRITE_SHADOW,
};

const tag_specific_type_t hf_tag_specific_types[] = {
    // Specific and necessary signs do not exist
    TAG_TYPE_UNDEFINED,
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
    for (uint32_t i = 0; i < ARRAY_SIZE(tag_type_names); i++) {
        if (tag_type_names[i].tag_type == tag_type) {
            return &(tag_type_names[i]);
        }
    }
    return NULL;
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
    tag_emulation_get_specific_types_by_slot(slot, tag_type);
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
    if (write_mode >= 0 && write_mode < 4) {
        return getLangString(mf1_write_mode_names[write_mode]);
    }
    return "";
}

void tag_helper_get_nickname(char *buff, size_t buff_len) {
    uint8_t slot = tag_emulation_get_slot();
    vfs_meta_t meta = {0};
    memset(buff, 0, buff_len);
    int32_t err = fds_read_meta(FDS_SLOT_TAG_DUMP_FILE_ID_BASE + slot, TAG_SENSE_HF, &meta);

    if (err == 0 && meta.has_notes) {
        strncpy(buff, meta.notes, buff_len);
    }

    if (strlen(buff) == 0) {
        sprintf(buff, "%s %02d", _T(APP_CHAMELEON_CARD_SLOT), slot + 1);
    }
}

int32_t tag_helper_set_nickname(const char *nickname) {
    uint8_t slot = tag_emulation_get_slot();
    vfs_meta_t meta = {0};

    int32_t err = fds_read_meta(FDS_SLOT_TAG_DUMP_FILE_ID_BASE + slot, TAG_SENSE_HF, &meta);
    if (err != 0) {
        return err;
    }
    meta.has_notes = 1;
    strcpy(meta.notes, nickname);
    return fds_write_meta(FDS_SLOT_TAG_DUMP_FILE_ID_BASE + slot, TAG_SENSE_HF, &meta);

    return -1;
}

size_t tag_helper_get_active_tag_data_size() {
    tag_specific_type_t tag_type = tag_helper_get_active_tag_type();
    const tag_specific_type_name_t *tag_type_name = tag_helper_get_tag_type_name(tag_type);
    return tag_type_name->data_size;
}

uint8_t *tag_helper_get_active_tag_memory_data() {
    tag_specific_type_t tag_type = tag_helper_get_active_tag_type();
    tag_data_buffer_t *tag_buffer = get_buffer_by_tag_type(tag_type);
    tag_group_type_t tag_group_type = tag_helper_get_tag_group_type(tag_type);
    if (tag_group_type == TAG_GROUP_MIFLARE) {
        nfc_tag_mf1_information_t *m_tag_information = (nfc_tag_mf1_information_t *)tag_buffer->buffer;
        return &m_tag_information->memory;
    } else {
        nfc_tag_ntag_information_t *m_tag_information = (nfc_tag_ntag_information_t *)tag_buffer->buffer;
        return &m_tag_information->memory;
    }
}

void tag_helper_generate_uid() {
    tag_specific_type_t tag_type = tag_helper_get_active_tag_type();
    tag_group_type_t tag_group_type = tag_helper_get_tag_group_type(tag_type);
    tag_data_buffer_t *tag_buffer = get_buffer_by_tag_type(tag_type);
    if (tag_group_type == TAG_GROUP_NTAG) {
        nfc_tag_ntag_information_t *m_tag_information = (nfc_tag_ntag_information_t *)tag_buffer->buffer;
        uint8_t uuid[7];
        ret_code_t err_code = utils_rand_bytes(uuid, sizeof(uuid));
        if (err_code == NRF_SUCCESS) {
            uuid[0] = 04; // fixed
            m_tag_information->memory[0][0] = uuid[0]; 
            m_tag_information->memory[0][1] = uuid[1];
            m_tag_information->memory[0][2] = uuid[2];
            // BCC 0 is always equal to UID0 ⊕ UID 1 ⊕ UID 2 ⊕ 0x88
            m_tag_information->memory[0][3] = m_tag_information->memory[0][0] ^ m_tag_information->memory[0][1] ^
                                              m_tag_information->memory[0][2] ^ 0x88;

            m_tag_information->memory[1][0] = uuid[3];
            m_tag_information->memory[1][1] = uuid[4];
            m_tag_information->memory[1][2] = uuid[5];
            m_tag_information->memory[1][3] = uuid[6];

            // BCC 1 is always equal to UID3 ⊕ UID 4 ⊕ UID 5 ⊕ UID6
            m_tag_information->memory[2][0] = m_tag_information->memory[1][0] ^ m_tag_information->memory[1][1] ^
                                              m_tag_information->memory[1][2] ^ m_tag_information->memory[1][3];

            memcpy(m_tag_information->res_coll.uid, uuid, sizeof(uuid));
        }
    }
}