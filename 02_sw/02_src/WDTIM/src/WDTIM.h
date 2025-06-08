/// @file WDTIM.h
/// @brief Header file used for configuring the watchdog timer
/// @author Aleksandra Petrovic

#ifndef WDTIM_H_
#define WDTIM_H_

#include "stm32f439xx.h"

void WDTIM_v_Configure(uint32_t u_PrescalerValue, uint32_t u_ReloadValue);
void WDTIM_v_Reload(void);
void WDTIM_v_Start(void);

#endif /* WDTIM_H_ */
