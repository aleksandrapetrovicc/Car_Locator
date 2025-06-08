/// @file LEDM.c
/// @brief Main file used for LED manipulation
/// @author Aleksandra Petrovic

#include "LEDM_cfg.h"
#include "LEDM.h"
#include "MONITOR.h"

static uint32_t LEDM_u_NumOfActivations = 0u;

void LEDM_v_Main()
{
  if (LEDM_u_NumOfActivations * LEDM_MAINFUNC_PERIOD >= LEDM_LED_TOGGLE_PERIOD)
  {
    // Toggle pin value
    HAL_GPIO_TogglePin(LEDM_REGISTER_GROUP, LEDM_REGISTER_PIN);

    LEDM_u_NumOfActivations = 0u;
  }
  else
  {
    LEDM_u_NumOfActivations++;
  }
}
