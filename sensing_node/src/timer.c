#include "em_core.h"
#include "em_cmu.h"
#include "em_letimer.h"
#include "common.h"
#include "timer.h"

void timers_en_ULFRCO()
{
  CMU_OscillatorEnable(cmuOsc_ULFRCO, true, true);

  CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_ULFRCO);

  CMU_ClockEnable(cmuClock_LFA, true);

  CMU_ClockDivSet(cmuClock_LETIMER0, cmuClkDiv_1);

  CMU_ClockEnable(cmuClock_LETIMER0, true);
}

void timers_en_LFXO()
{
  CMU_OscillatorEnable(cmuOsc_LFXO, true, true);

  CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_LFXO);

  CMU_ClockEnable(cmuClock_LFA, true);

  // Pre-scaling helps in maintaining comp0 and comp1 value under 16bit
  // range for 7 second time-period
  CMU_ClockDivSet(cmuClock_LETIMER0, cmuClkDiv_4);

  CMU_ClockEnable(cmuClock_LETIMER0, true);
}

void timers_init()
{
  timers_en_ULFRCO();
}

void timers_set(uint16_t inter_period_ms, uint16_t period_ms)
{
  // Calculated COMP0 and COMP1 values for given LED ON TIME and LED PERIOD
  // are stored in comp0_counter and comp1_counter variables respectively
  uint16_t comp0_counter = 0;
  uint16_t comp1_counter = 0;

  comp0_counter = (period_ms * CMU_ClockFreqGet(cmuClock_LETIMER0)) / 1000;
  comp1_counter = (inter_period_ms * CMU_ClockFreqGet(cmuClock_LETIMER0)) / 1000;

  const LETIMER_Init_TypeDef letimerInit =
      {
          .enable         = false,
          .debugRun       = false,
          .comp0Top       = false,
          .bufTop         = false,
          .out0Pol        = 0,
          .out1Pol        = 0,
          .ufoa0          = letimerUFOANone,
          .ufoa1          = letimerUFOANone,
          .repMode        = letimerRepeatFree,
          .topValue        = comp0_counter
      };

  if (comp0_counter && comp1_counter)
    {
      LETIMER_CompareSet(LETIMER0, 1 , comp1_counter);
      LETIMER_CompareSet(LETIMER0, 0 , comp0_counter);
      LETIMER_IntEnable(LETIMER0, LETIMER_IEN_UF | LETIMER_IEN_COMP1);
    }
  else if (comp0_counter) {
      LETIMER_CompareSet(LETIMER0, 0 , comp0_counter);
      LETIMER_IntEnable(LETIMER0, LETIMER_IEN_UF);
  }
  else {
      LETIMER_IntDisable(LETIMER0, LETIMER_IEN_UF | LETIMER_IEN_COMP1);
  }

  LETIMER_Init(LETIMER0, &letimerInit);

  LETIMER_Enable(LETIMER0, true);
}

void LETIMER0_IRQHandler(void)
{
  // Get value of pending interrupts in LETIMER0
  uint32_t reason = LETIMER_IntGetEnabled(LETIMER0);

  // Clear pending interrupts in LETIMER0
  LETIMER_IntClear(LETIMER0, reason);

  if (reason & LETIMER_IEN_UF) {
      sl_bt_external_signal(EVT_READ_TEMP);
  }

  if (reason & LETIMER_IEN_COMP1) {

  }
}
