/*
 * ble.h
 *
 *  Created on: 2021年9月28日
 *      Author: solos
 */

#ifndef BLE_H_
#define BLE_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

//#define DEVICE_NAME "amiibolink" /**< Name of device. Will be included in the advertising data. */

#define MAX_MTU_DAT_SIZE (NRF_SDH_BLE_GATT_MAX_MTU_SIZE-3)


typedef enum {
    BLE_DEVICE_MODE_PIXLJS,
    BLE_DEVICE_MODE_AMIIBOLINK,
    BLE_DEVICE_MODE_AMIIBOLINK_V2,
    BLE_DEVICE_MODE_AMILOOP
} ble_device_mode_t;

typedef void (*nus_rx_data_handler_t)(const uint8_t *data, size_t data_len);
typedef void (*nus_tx_ready_handler_t)(void);
typedef void (*ble_raw_adv_done_cb_t)(void);
typedef void (*ble_conn_evt_cb_t)(bool connected);


void ble_init(void);
void ble_device_mode_prepare(ble_device_mode_t mode);
void ble_adv_start(void);
void ble_disable();

uint32_t ble_raw_adv_start(const uint8_t mac_be[6], const uint8_t *adv_data, uint8_t adv_len,
                           bool connectable, uint16_t duration_10ms, ble_raw_adv_done_cb_t done_cb);
void ble_raw_adv_stop(void);
bool ble_raw_adv_is_active(void);

typedef struct {
    uint8_t controller_mac[6];
    uint8_t host_mac[6];
    uint16_t pid;
    uint8_t adv_data[31];
    uint8_t adv_len;
} ble_switch2_scan_result_t;

typedef void (*ble_switch2_scan_cb_t)(bool found, const ble_switch2_scan_result_t *result);

uint32_t ble_switch2_scan_start(uint16_t timeout_sec, ble_switch2_scan_cb_t cb);
void ble_switch2_scan_stop(void);
bool ble_switch2_scan_is_active(void);


void ble_nus_set_handler(nus_rx_data_handler_t rx_data_handler, nus_tx_ready_handler_t tx_ready_handler);
void ble_set_conn_evt_handler(ble_conn_evt_cb_t cb);
uint32_t ble_nus_tx_data(void *data, size_t length);
bool ble_get_rssi(int8_t *p_rssi);
void ble_get_addr_str(char addr_fmt[]);



#endif /* BLE_H_ */
