/*
 * si7021.h
 *
 *  Created on: Oct 16, 2023
 *      Author: ajayk
 */

#ifndef SRC_SI7021_H_
#define SRC_SI7021_H_

void  si7021_init     ();
float si7021_read_temp();
uint8_t si7021_get_curr_temp(void);
uint8_t si7021_get_target_temp(void);
uint8_t si7021_get_auto_ctrl(void);
void  si7021_evt_proc (sl_bt_msg_t *evt);

#endif /* SRC_SI7021_H_ */
