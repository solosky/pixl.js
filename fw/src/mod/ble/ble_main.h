/*
 * ble.h
 *
 *  Created on: 2021年9月28日
 *      Author: solos
 */

#ifndef BLE_H_
#define BLE_H_

//#define DEVICE_NAME "amiibolink" /**< Name of device. Will be included in the advertising data. */

#define MAX_MTU_DAT_SIZE (NRF_SDH_BLE_GATT_MAX_MTU_SIZE-3)


typedef enum {
    BLE_DEVICE_MODE_PIXLJS,
    BLE_DEVICE_MODE_AMIIBOLINK,
    BLE_DEVICE_MODE_AMIIBOLINK_V2
} ble_device_mode_t;

typedef void (*nus_rx_data_handler_t)(const uint8_t *data, size_t data_len);
typedef void (*nus_tx_ready_handler_t)(void);


void ble_init(void);
void ble_device_mode_prepare(ble_device_mode_t mode);
void ble_adv_start(void);
void ble_disable();


void ble_nus_set_handler(nus_rx_data_handler_t rx_data_handler, nus_tx_ready_handler_t tx_ready_handler);
uint32_t ble_nus_tx_data(void *data, size_t length);
void ble_get_addr_str(char addr_fmt[]);



#endif /* BLE_H_ */
