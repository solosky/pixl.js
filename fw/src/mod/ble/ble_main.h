/*
 * ble.h
 *
 *  Created on: 2021年9月28日
 *      Author: solos
 */

#ifndef BLE_H_
#define BLE_H_


void ble_init(void);
void ble_disable();



uint32_t ble_nus_tx_data(void *data, size_t length);
void ble_get_addr_str(char addr_fmt[]);



#endif /* BLE_H_ */
