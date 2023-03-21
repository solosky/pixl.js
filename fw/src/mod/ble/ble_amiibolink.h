#ifndef BLE_AMIIBOLINK_H
#define BLE_AMIIBOLINK_H

#include <stddef.h>

void ble_amiibolink_init(void);
void ble_amiibolink_received_data(const void* data, size_t length);

#endif