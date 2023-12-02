/*
 * common.h
 *
 *  Created on: 21-Nov-2023
 *      Author: ajayk
 */

#ifndef SRC_COMMON_H_
#define SRC_COMMON_H_

#include "sl_bluetooth.h"

#define I2C0_SCL_port gpioPortC
#define I2C0_SCL_pin 8
#define I2C0_SDA_port gpioPortC
#define I2C0_SDA_pin 7
#define I2C0_SCL_portLocation 12
#define I2C0_SDA_portLocation 12

//#define I2C0_SCL_port gpioPortC
//#define I2C0_SCL_pin 7
//#define I2C0_SDA_port gpioPortC
//#define I2C0_SDA_pin 8
//#define I2C0_SCL_portLocation 11
//#define I2C0_SDA_portLocation 13

#define ENCODER_RA_PORT gpioPortD
#define ENCODER_RA_PIN 12
#define ENCODER_RB_PORT gpioPortD
#define ENCODER_RB_PIN 11
#define ENCODER_R1_PORT gpioPortD
#define ENCODER_R1_PIN 10

typedef enum {
  BT_CODE_NULL,
  BT_CODE_TURN_ON,
  BT_CODE_TURN_OFF,
  BT_CODE_AUTO_ON,
  BT_CODE_AUTO_OFF,
  BT_CODE_TEMP_EQ_TARGET,
  BT_CODE_TEMP_GT_TARGET,
  BT_CODE_TEMP_LT_TARGET,
  BT_CODE_LUMEN_LESS,
  BT_CODE_LUMEN_MODERATE,
  BT_CODE_LUMEN_HIGH,
  BT_CODE_TURN_ON_HEATER,
  BT_CODE_TURN_OFF_HEATER,
  BT_CODE_TURN_ON_AC,
  BT_CODE_TURN_OFF_AC,
  BT_CODE_TURN_ON_LIGHT,
  BT_CODE_TURN_OFF_LIGHT
} bt_control_codes_t;

typedef enum {
  EVT_RELAY_ON = 0x01,
  EVT_RELAY_OFF = 0x02,
  EVT_READ_TEMP = 0x04,
  EVT_ENCODER_BUTTON = 0x08,
  EVT_ENCODER_CLK = 0x10,
  EVT_ENCODER_ACLK = 0x20,
  EVT_AUTO_ON = 0x40,
  EVT_AUTO_OFF = 0x80
} evt_t;

typedef struct {
  bd_addr address;
  uint8_t address_type;
  uint8_t advertising_handle;
  uint8_t connection_handle;
  uint8_t indication_en;
  uint8_t value;
  uint8_t indication_in_flight;
}ble_data_t;

#endif /* SRC_COMMON_H_ */
