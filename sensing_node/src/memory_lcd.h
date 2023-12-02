/***************************************************************************//**
 * @file memlcd_app.h
 * @brief Memory Liquid Crystal Display (LCD) example functions
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#ifndef MEMLCD_APP_H
#define MEMLCD_APP_H

/***************************************************************************//**
 * Initialize example
 ******************************************************************************/
void memlcd_init(void);

void memlcd_update(uint8_t data, uint8_t data_type);

/***************************************************************************//**
 * Ticking function
 ******************************************************************************/
void memlcd_evt_proc(sl_bt_msg_t *evt);

#endif // MEMLCD_APP_H
