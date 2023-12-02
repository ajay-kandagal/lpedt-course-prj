/*
 * relay.h
 *
 *  Created on: Oct 16, 2023
 *      Author: ajayk
 */

#ifndef SRC_RELAY_H_
#define SRC_RELAY_H_

#include "sl_bluetooth.h"

void relay_init();
void relay_on();
void relay_off();
void relay_evt_proc(sl_bt_msg_t *evt);

#endif /* SRC_RELAY_H_ */
