/*
 * ble_handler.h
 *
 *  Created on: 21-Nov-2023
 *      Author: ajayk
 */

#ifndef SRC_BLE_HANDLER_H_
#define SRC_BLE_HANDLER_H_

#include "sl_bluetooth.h"
#include "common.h"

void ble_handler_init();
ble_data_t* ble_handler_get_inst();
void ble_handler_evt_proc(sl_bt_msg_t *evt);

#endif /* SRC_BLE_HANDLER_H_ */
