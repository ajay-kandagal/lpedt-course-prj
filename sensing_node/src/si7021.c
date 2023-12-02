/*
 * @file   si7021.c
 * @brief  File contains functions to initialize the GPIOs required for
 *         I2C0 required for communication with si7021. Reads 14-bit temperature
 *         value and returns it.
 *
 * @date   Oct 13, 2023
 * @authoe Ajay Kandahal
 */
#include "em_i2c.h"
#include "em_core.h"
#include "em_cmu.h"
#include "gatt_db.h"
#include "sl_i2cspm.h"
#include "sl_udelay.h"
#include "app_log.h"

#include "common.h"
#include "ble_handler.h"
#include "si7021.h"

#define SI7021_DEV_ADDR        (0x40)
#define SI7021_NOHOLD_TEMP_CMD (0xF3)

static uint8_t target_temp = 25;
static uint8_t current_temp = 25;
static uint8_t auto_ctrl = 1;

void si7021_init()
{
  I2CSPM_Init_TypeDef I2C_config = {
      .port = I2C0,
      .sclPort = I2C0_SCL_port,
      .sclPin = I2C0_SCL_pin,
      .sdaPort = I2C0_SDA_port,
      .sdaPin = I2C0_SDA_pin,
      .portLocationScl = I2C0_SCL_portLocation,
      .portLocationSda = I2C0_SDA_portLocation,
      .i2cRefFreq = 0,
      .i2cMaxFreq = I2C_FREQ_STANDARD_MAX,
      .i2cClhr = i2cClockHLRStandard
  };

  I2CSPM_Init(&I2C_config);
}

float si7021_read_temp(void)
{
  // Send Measure no-hold temperature measurement command
  I2C_TransferReturn_TypeDef transferStatus;
  I2C_TransferSeq_TypeDef    transferSequence;
  uint8_t                    cmd_data;
  uint8_t                    read_data[2];
  uint16_t                   temp_raw;
  float                      temp_in_c;

  // Wait for 80ms for device to ready for I2C transaction
  // USTIMER_Delay(80000);

  cmd_data                     = SI7021_NOHOLD_TEMP_CMD;
  transferSequence.addr        = SI7021_DEV_ADDR << 1;
  transferSequence.flags       = I2C_FLAG_WRITE;
  transferSequence.buf[0].data = &cmd_data;
  transferSequence.buf[0].len  = sizeof(cmd_data);

  transferStatus = I2CSPM_Transfer(I2C0, &transferSequence);

  if (transferStatus != i2cTransferDone)
    app_log_info("Failed to send command to si7021 %x", transferStatus);

  // Wait 10.8ms for 14-bit temperature measurement
  sl_udelay_wait(10800);

  // Read 2-bytes of temperature measurement
  transferSequence.flags       = I2C_FLAG_READ;
  transferSequence.buf[0].data = (uint8_t*) &read_data;
  transferSequence.buf[0].len  = sizeof(read_data);

  transferStatus = I2CSPM_Transfer(I2C0, &transferSequence);

  if (transferStatus != i2cTransferDone)
    app_log_info("Failed to read temperature from si7021 %x", transferStatus);

  temp_raw = read_data[1] | read_data[0] << 8;
  temp_in_c = ((temp_raw * 175.72) / 65535) - 46.85;

  return temp_in_c;
}

uint8_t si7021_get_curr_temp(void)
{
  return current_temp;
}

uint8_t si7021_get_target_temp(void)
{
  return target_temp;
}

uint8_t si7021_get_auto_ctrl(void)
{
  return auto_ctrl;
}

void si7021_evt_proc(sl_bt_msg_t *evt)
{
  if (SL_BT_MSG_ID(evt->header) != sl_bt_evt_system_external_signal_id)
    return;

  ble_data_t* ble_handle = ble_handler_get_inst();

  switch(evt->data.evt_system_external_signal.extsignals)
  {
    case EVT_READ_TEMP:
      current_temp = (uint8_t)si7021_read_temp();
      break;

    case EVT_ENCODER_CLK:
      if (target_temp < 40)
        target_temp++;

      auto_ctrl = 0;
      break;

    case EVT_ENCODER_ACLK:
      if (target_temp > 18)
        target_temp--;

      auto_ctrl = 0;
      break;

    case EVT_ENCODER_BUTTON:
      auto_ctrl = !auto_ctrl;
      break;

    case EVT_AUTO_ON:
      auto_ctrl = 1;
      break;

    case EVT_AUTO_OFF:
      auto_ctrl = 0;
      break;

    default:
      break;
  }

  if (current_temp > target_temp)
    ble_handle->value = BT_CODE_TEMP_GT_TARGET;
  else if (current_temp < target_temp)
    ble_handle->value = BT_CODE_TEMP_LT_TARGET;
  else
    ble_handle->value = BT_CODE_TEMP_EQ_TARGET;

  sl_bt_gatt_server_write_attribute_value(gattdb_relay_control, 0, sizeof(ble_handle->value), &ble_handle->value);

  if (ble_handle->indication_en && !ble_handle->indication_in_flight && auto_ctrl) {
      sl_bt_gatt_server_send_indication(ble_handle->connection_handle, gattdb_relay_control, sizeof(ble_handle->value), &ble_handle->value);
      ble_handle->indication_in_flight = true;
  }
}
