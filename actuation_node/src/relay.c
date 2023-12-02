/*
 * relay.c
 *
 *  Created on: Oct 16, 2023
 *      Author: ajayk
 */
#include "em_core.h"
#include "em_cmu.h"
#include "sl_bluetooth.h"
#include "sl_udelay.h"

#include "common.h"
#include "relay.h"

void relay_init()
{
  GPIO_PinModeSet(RELAY_CTRL_PORT, RELAY_CTRL_EN_PIN, gpioModePushPull, false);
  GPIO_PinModeSet(RELAY_CTRL_PORT, RELAY_CTRL_SET_PIN, gpioModePushPull, false);
  GPIO_PinModeSet(RELAY_CTRL_PORT, RELAY_CTRL_RESET_PIN, gpioModePushPull, false);

  relay_off();
}

void relay_on()
{
  // Enable the relay driver
  GPIO_PinOutSet(RELAY_CTRL_PORT, RELAY_CTRL_EN_PIN);

  //Turn on the relay by moving the charge from A1 to A2
  GPIO_PinOutSet(RELAY_CTRL_PORT, RELAY_CTRL_SET_PIN);
  GPIO_PinOutClear(RELAY_CTRL_PORT, RELAY_CTRL_RESET_PIN);

  // Wait for operating time of relay as mentioned in datasheet
  sl_udelay_wait(4000);

  //Turn off the relay coil
  GPIO_PinOutClear(RELAY_CTRL_PORT, RELAY_CTRL_SET_PIN);
  GPIO_PinOutClear(RELAY_CTRL_PORT, RELAY_CTRL_RESET_PIN);

  // Enable the relay driver
  GPIO_PinOutClear(RELAY_CTRL_PORT, RELAY_CTRL_EN_PIN);
}

void relay_off()
{
  // Enable the relay driver
  GPIO_PinOutSet(RELAY_CTRL_PORT, RELAY_CTRL_EN_PIN);

  //Turn on the relay by moving the charge from A2 to A1
  GPIO_PinOutSet(RELAY_CTRL_PORT, RELAY_CTRL_RESET_PIN);
  GPIO_PinOutClear(RELAY_CTRL_PORT, RELAY_CTRL_SET_PIN);

  // Wait for operating time of relay as mentioned in datasheet
  sl_udelay_wait(4000);

  //Turn off the relay coil
  GPIO_PinOutClear(RELAY_CTRL_PORT, RELAY_CTRL_SET_PIN);
  GPIO_PinOutClear(RELAY_CTRL_PORT, RELAY_CTRL_RESET_PIN);

  // Enable the relay driver
  GPIO_PinOutClear(RELAY_CTRL_PORT, RELAY_CTRL_EN_PIN);
}

void relay_evt_proc(sl_bt_msg_t *evt)
{
  if (SL_BT_MSG_ID(evt->header) != sl_bt_evt_system_external_signal_id)
    return;

  switch(evt->data.evt_system_external_signal.extsignals)
  {
    case EVT_RELAY_ON:
      relay_on();
      break;
    case EVT_RELAY_OFF:
      relay_off();
      break;
    default:
      break;
  }
}
