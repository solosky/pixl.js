#ifndef BLE_DF_DRIVER_H
#define BLE_DF_DRIVER_H


#include "df_driver.h"
#include <stddef.h>

df_driver_t* ble_get_df_driver();


void ble_on_received_data(const uint8_t* data, size_t length);
void ble_on_transmit_ready();
#endif 