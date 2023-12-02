/*
 * encoder.c
 *
 *  Created on: 24-Nov-2023
 *      Author: ajayk
 */
#include "em_core.h"
#include "em_cmu.h"

#include "common.h"
#include "encoder.h"

#define EVEN_R1_FLAG 1024
#define EVEN_RA_FLAG 4096
#define ODD_RB_FLAG  0x0800

static volatile uint32_t last_flag = 0;
static volatile uint32_t last_event = 0;

void encoder_init()
{
  GPIO_PinModeSet(ENCODER_RA_PORT, ENCODER_RA_PIN, gpioModeInputPull, true);
  GPIO_PinModeSet(ENCODER_RB_PORT, ENCODER_RB_PIN, gpioModeInputPull, true);
  GPIO_PinModeSet(ENCODER_R1_PORT, ENCODER_R1_PIN, gpioModeInputPull, true);

  GPIO_ExtIntConfig(ENCODER_RA_PORT, ENCODER_RA_PIN, ENCODER_RA_PIN, false, true, true);
  GPIO_ExtIntConfig(ENCODER_RB_PORT, ENCODER_RB_PIN, ENCODER_RB_PIN, false, true, true);
  GPIO_ExtIntConfig(ENCODER_R1_PORT, ENCODER_R1_PIN, ENCODER_R1_PIN, false, true, true);
}

void GPIO_EVEN_IRQHandler()
{
  uint32_t flags = GPIO_IntGet();
  last_event = flags;
  GPIO_IntClear(flags);

  if (flags == EVEN_R1_FLAG) {
      last_flag = 0;
      sl_bt_external_signal(EVT_ENCODER_BUTTON);
  }
  else if (flags == EVEN_RA_FLAG) {
      if (last_flag == ODD_RB_FLAG) {
          last_flag = 0;
          sl_bt_external_signal(EVT_ENCODER_ACLK);
      }
      else {
          last_flag = EVEN_RA_FLAG;
      }
  }
}

void GPIO_ODD_IRQHandler()
{
  uint32_t flags = GPIO_IntGet();
  last_event = flags;
  GPIO_IntClear(flags);

  if (flags == ODD_RB_FLAG){
      if (last_flag == EVEN_RA_FLAG) {
          last_flag = 0;
          sl_bt_external_signal(EVT_ENCODER_CLK);
      }
      else {
          last_flag = ODD_RB_FLAG;
      }
  }
}

