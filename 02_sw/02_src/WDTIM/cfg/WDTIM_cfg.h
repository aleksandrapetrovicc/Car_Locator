/// @file WDTIM_cfg.h
/// @brief Contains configuration data used for watchdog timer
/// @author Aleksandra Petrovic

#ifndef WDTIM_CFG_H_
#define WDTIM_CFG_H_

#include "Registers.h"
#include "WDTIM.h"

/// To modify IWDG_PR and IWDG_RLR, 0x5555 in IWDG_KR register must be written first
#define IWDG_KR_CFG_ENABLE 0x5555
/// Value 0xAAAA written by software resets watchdog timer
#define IWDG_KR_RELOAD 0xAAAA
/// Value 0xCCCC written by software start watchdog timer
#define IWDG_START 0xCCCC

#endif /* WDTIM_CFG_H_ */
