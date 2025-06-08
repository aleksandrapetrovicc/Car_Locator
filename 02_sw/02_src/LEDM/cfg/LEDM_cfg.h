/// @file LEDM_cfg.h
/// @brief Contains configuration data used for manipulating with LED
/// @author Aleksandra Petrovic

#ifndef LEDM_CFG_H_
#define LEDM_CFG_H_

#include "stm32f439xx.h"
#include "stm32f4xx_hal.h"
#include "main.h"

/// Period of the LEDM OS task
#define LEDM_MAINFUNC_PERIOD (PERIOD_TSK_LED)

/// Toggle period in microseconds
#define LEDM_LED_TOGGLE_PERIOD 500u

/// Register address used for toggling pin output
#define LEDM_REGISTER_GROUP LD2_GPIO_Port

/// Pin position in register
#define LEDM_REGISTER_PIN LD2_Pin

#endif /* LEDM_CFG_H_ */
