#ifndef BLE_AMIIBOLINK_H
#define BLE_AMIIBOLINK_H

#include <stddef.h>
#include <stdint.h>

typedef enum {
    BLE_AMIIBOLINK_MODE_RANDOM = 1,
    BLE_AMIIBOLINK_MODE_CYCLE = 2,
    BLE_AMIIBOLINK_MODE_NTAG = 3
} ble_amiibolink_mode_t;

typedef struct {
    uint8_t key1;
    uint8_t key2;
    uint8_t data_len;
    uint8_t de_data_len;
    uint8_t data[32];
} link_data_t;

typedef enum { BLE_AMIIBOLINK_EVENT_TAG_UPDATED, BLE_AMIIBOLINK_EVENT_SET_MODE } ble_amiibolink_event_type_t;

typedef void (*ble_amiibolink_event_handler_t)(void* ctx, ble_amiibolink_event_type_t, uint8_t *, size_t);

void ble_amiibolink_init(void);
void ble_amiibolink_set_event_handler(ble_amiibolink_event_handler_t handler, void* context);
void ble_amiibolink_received_data(const uint8_t *data, size_t length);

#endif