#include "ble_amiibolink.h"
#include "ble_main.h"
#include "df_buffer.h"
#include "nrf_log.h"
#include "ntag_def.h"
#include "ntag_emu.h"

#include "nrf_crypto.h"
#include "nrf_crypto_aes.h"

static ntag_t m_ntag = {0};
uint32_t m_data_pos = 0;
ble_amiibolink_event_handler_t m_event_handler = {0};
void *m_event_ctx = {0};
ble_amiibolink_ver_t m_ver = BLE_AMIIBOLINK_VER_V2;

void ble_amiibolink_set_event_handler(ble_amiibolink_event_handler_t handler, void *ctx) {
    m_event_handler = handler;
    m_event_ctx = ctx;
}


void ble_amiibolink_set_version(ble_amiibolink_ver_t ver){
    m_ver = ver;
}
void pad16(uint16_t data, uint8_t buf[16]) {
    memset(buf, 0, 16);
    memcpy(buf, &data, sizeof(uint16_t));
}



void ble_amiibolink_send_cmd_v1(uint16_t cmd){
    ble_nus_tx_data(&cmd, 2);
}

void ble_amiibolink_send_cmd_v2(uint16_t cmd) {
    link_data_t link_data = {0};
    link_data.key1 = (rand() % 256) + 1;
    link_data.key2 = (rand() % 256) + 1;
    link_data.data_len = 16;
    link_data.de_data_len = 2;

    uint8_t pad[16] = {0};
    pad16(cmd, pad);

    uint8_t key[] = {
        0x4B, 0x47, 0x46, 0x5F, 0x41, 0x4D, 0x49,
        0x4C, 0x07, 0xE7, 0x04, 0x06, 0x0A, 0x2A,
        link_data.key1, link_data.key2 // 随机密钥
    };

    nrf_crypto_aes_context_t aes_ecb_ctx;
    size_t out_len = 16;
    ret_code_t err = nrf_crypto_aes_crypt(
        &aes_ecb_ctx,
        &g_nrf_crypto_aes_ecb_128_info,
        NRF_CRYPTO_ENCRYPT,
        &key,
        NULL,
        pad,
        16,
        link_data.data,
        &out_len
    );
    NRF_LOG_INFO("nrf_crypto_aes_encrypt:out_len=%d err=%d msg=%s", out_len, err, nrf_crypto_error_string_get(err));

    ble_nus_tx_data(&link_data, sizeof(link_data_t));
}

void ble_amiibolink_send_cmd(uint16_t cmd) {
    if (m_ver == BLE_AMIIBOLINK_VER_V1) {
        ble_amiibolink_send_cmd_v1(cmd);
    } else {
        ble_amiibolink_send_cmd_v2(cmd);
    }
}


void ble_amiibolink_write_ntag(buffer_t *buffer) {
    buff_get_u8(buffer); // 00
    uint8_t size = buff_get_u8(buffer);
    void *data_ptr = buff_get_data_ptr_pos(buffer);
    memcpy(m_ntag.data + m_data_pos, data_ptr, size);
    m_data_pos += size;
}

void ble_amiibolink_init(void) {}

void ble_amiibolink_process_cmd(buffer_t* buffer){

    ble_amiibolink_mode_t mode;
    uint16_t cmd = buff_get_u16(buffer);
    switch (cmd) {

    case 0xB1A1: // send model code ??
        // a1 b1 01
        // 01: 随机模式 02: 按序模式 03:读写模式
        mode = (ble_amiibolink_mode_t)buff_get_u8(buffer);
        if (m_event_handler) {
            m_event_handler(m_event_ctx, BLE_AMIIBOLINK_EVENT_SET_MODE, &mode, sizeof(ble_amiibolink_mode_t));
        }
        ble_amiibolink_send_cmd(0xA1B1);
        break;

    case 0xB0A0: // seq 1
        ble_amiibolink_send_cmd(0xA0B0);
        break;

    case 0xACAC: // seq 2
        // ac ac 00 04 00 00 02 1c //540
        m_data_pos = 0;
        ble_amiibolink_send_cmd(0xCACA);
        break;

    case 0xABAB: // seq 3
        // ab ab 02 1c
        ble_amiibolink_send_cmd(0xBABA);
        break;

    case 0xAADD: // seq 4
        ble_amiibolink_write_ntag(buffer);
        ble_amiibolink_send_cmd(0xDDAA);
        break;

    case 0xBCBC: // seq 5
        ble_amiibolink_send_cmd(0xCBCB);
        break;

    case 0xDDCC: // seq 6
        NRF_LOG_INFO("ntag_emu_set_tag");
        ntag_emu_set_tag(&m_ntag);
        if (m_event_handler) {
            m_event_handler(m_event_ctx, BLE_AMIIBOLINK_EVENT_TAG_UPDATED, &m_ntag, sizeof(m_ntag));
        }
        ble_amiibolink_send_cmd(0xCCDD);
        break;
    }
}


void ble_amiibolink_received_data_v1(const uint8_t *data, size_t length){
    NEW_BUFFER_READ(buffer, (void *)data, length);
    ble_amiibolink_process_cmd(&buffer);
}


void ble_amiibolink_received_data_v2(const uint8_t *data, size_t length) {
    NRF_LOG_INFO("ble data received %d bytes", length);
    //NRF_LOG_HEXDUMP_INFO(data, length);

    //  13   45     10       02     76 98 8D 3D.....
    // key1 key2 datalen dedatalen data
    link_data_t *link_data = (link_data_t *) data;

    nrf_crypto_aes_context_t aes_ecb_ctx;

    uint8_t buf[MAX_MTU_DAT_SIZE];
    size_t out_len = sizeof(buf);
    size_t data_len = link_data->de_data_len;

    uint8_t key[] = {
        0x4B, 0x47, 0x46, 0x5F, 0x41, 0x4D, 0x49,
        0x4C, 0x07, 0xE7, 0x04, 0x06, 0x0A, 0x2A,
        link_data->key1, link_data->key2 // 随机密钥
    };

    ret_code_t err = nrf_crypto_aes_crypt(
        &aes_ecb_ctx,
        &g_nrf_crypto_aes_ecb_128_info,
        NRF_CRYPTO_DECRYPT,
        &key,
        NULL,
        link_data->data,
        link_data->data_len,
        buf,
        &out_len
    );

    NRF_LOG_INFO("decrypted data len: de_data_len=%d, decrypted_len=%d", link_data->de_data_len, out_len);
    NRF_LOG_INFO("nrf_crypto_aes_decrypt: %d %s", err, nrf_crypto_error_string_get(err));
    NRF_LOG_HEXDUMP_DEBUG(&buf, data_len);

    NEW_BUFFER_READ(buffer, (void *) &buf, data_len);

    ble_amiibolink_process_cmd(&buffer);
}

void ble_amiibolink_received_data(const uint8_t *data, size_t length) {
    if (m_ver == BLE_AMIIBOLINK_VER_V1) {
        ble_amiibolink_received_data_v1(data, length);
    } else {
        ble_amiibolink_received_data_v2(data, length);
    }
}