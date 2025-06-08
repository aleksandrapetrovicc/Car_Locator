/// @file WDTIM.c
/// @brief Main file used for configuring the watchdog timer
/// @author Aleksandra Petrovic

#include "WDTIM_cfg.h"

void WDTIM_v_Configure(uint32_t u_PrescalerValue, uint32_t u_ReloadValue)
{
  // To modify IWDG_PR and IWDG_RLR, 0x5555 in IWDG_KR register must be written first
  REG32(IWDG_KR) = IWDG_KR_CFG_ENABLE;
  // Set prescaler value in PR register
  REG32(IWDG_PR) = u_PrescalerValue; //IWDG_PR_VALUE // Default is 0x00
  // Set watchdog counter reload value
  REG32(IWDG_RLR) = u_ReloadValue; //IWDG_RLR_VALUE  // Default is 0xff
}

void WDTIM_v_Reload()
{
  // These bits must be written by software at regular intervals with the key value 0xAAAA, otherwise the watchdog generates a reset when the counter reaches 0.
  REG32(IWDG_KR) = IWDG_KR_RELOAD;
}

void WDTIM_v_Start()
{
  // Writing 0xCCCC in KR register starts the watchdog timer
  REG32(IWDG_KR) = IWDG_START;
}
