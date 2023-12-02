/***************************************************************************//**
 * @file memlcd_app.c
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
#include <stdio.h>

#include "sl_board_control.h"
#include "em_assert.h"
#include "glib.h"
#include "dmd.h"
#include "em_cmu.h"
#include "sl_bluetooth.h"
#include "si7021.h"
#include "common.h"
/*******************************************************************************
 *******************************   DEFINES   ***********************************
 ******************************************************************************/

/*******************************************************************************
 ***************************  LOCAL VARIABLES   ********************************
 ******************************************************************************/
static GLIB_Context_t glibContext;

/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/

/***************************************************************************//**
 * Initialize example.
 ******************************************************************************/
void memlcd_init(void)
{
  uint32_t status;

  GPIO_PinModeSet(gpioPortA, 1, gpioModePushPull, 0);
  GPIO_PinModeSet(gpioPortA, 0, gpioModePushPull, 0);
  GPIO_PinOutSet(gpioPortA, 0);

  /* Disable the memory lcd */
  GPIO_PinOutClear(gpioPortA, 1);

  /* Initialize the DMD support for memory lcd display */
  status = DMD_init(0);
  EFM_ASSERT(status == DMD_OK);

  /* Initialize the glib context */
  status = GLIB_contextInit(&glibContext);
  EFM_ASSERT(status == GLIB_OK);

  glibContext.backgroundColor = White;
  glibContext.foregroundColor = Black;

  /* Fill lcd with background color */
  GLIB_clear(&glibContext);

  /* Use Narrow font */
  GLIB_setFont(&glibContext, (GLIB_Font_t *) &GLIB_FontNumber16x20);

  GLIB_drawStringOnLine(&glibContext,
                        "25",
                        0,
                        GLIB_ALIGN_CENTER,
                        0,
                        0,
                        true);

  GLIB_drawStringOnLine(&glibContext,
                        "25",
                        2,
                        GLIB_ALIGN_CENTER,
                        0,
                        0,
                        true);

  GLIB_setFont(&glibContext, (GLIB_Font_t *) &GLIB_FontNormal8x8);

  GLIB_drawStringOnLine(&glibContext,
                        "Auto ON",
                        9,
                        GLIB_ALIGN_CENTER,
                        0,
                        0,
                        true);

  DMD_updateDisplay();
  /* Enable the memory lcd */
  GPIO_PinOutSet(gpioPortA, 1);
}

/***************************************************************************//**
 * Ticking function.
 ******************************************************************************/
void memlcd_evt_proc(sl_bt_msg_t *evt)
{
  if (SL_BT_MSG_ID(evt->header) != sl_bt_evt_system_external_signal_id)
      return;

    char temp_data[6];

    switch(evt->data.evt_system_external_signal.extsignals)
    {
      case EVT_READ_TEMP:
        GPIO_PinOutClear(gpioPortA, 1);
        sprintf(temp_data, "%d", si7021_get_curr_temp());
        GLIB_setFont(&glibContext, (GLIB_Font_t *) &GLIB_FontNumber16x20);
        GLIB_drawStringOnLine(&glibContext,
                              temp_data,
                              0,
                              GLIB_ALIGN_CENTER,
                              0,
                              0,
                              true);
        DMD_updateDisplay();
        GPIO_PinOutSet(gpioPortA, 1);
        break;

      case EVT_ENCODER_CLK:
      case EVT_ENCODER_ACLK:
        GPIO_PinOutClear(gpioPortA, 1);
        sprintf(temp_data, "%d", si7021_get_target_temp());
        GLIB_setFont(&glibContext, (GLIB_Font_t *) &GLIB_FontNumber16x20);
        GLIB_drawStringOnLine(&glibContext,
                              temp_data,
                              2,
                              GLIB_ALIGN_CENTER,
                              0,
                              0,
                              true);

        GLIB_setFont(&glibContext, (GLIB_Font_t *) &GLIB_FontNormal8x8);
        GLIB_drawStringOnLine(&glibContext,
                              "Auto OFF",
                              9,
                              GLIB_ALIGN_CENTER,
                              0,
                              0,
                              true);
        DMD_updateDisplay();
        GPIO_PinOutSet(gpioPortA, 1);
        break;

      case EVT_AUTO_ON:
      case EVT_AUTO_OFF:
      case EVT_ENCODER_BUTTON:
        GLIB_setFont(&glibContext, (GLIB_Font_t *) &GLIB_FontNormal8x8);
        GLIB_drawStringOnLine(&glibContext,
                              si7021_get_auto_ctrl() ? "Auto ON " : "Auto OFF",
                              9,
                              GLIB_ALIGN_CENTER,
                              0,
                              0,
                              true);
        DMD_updateDisplay();
        GPIO_PinOutSet(gpioPortA, 1);
        break;

      default:
        break;
    }
    return;
}
