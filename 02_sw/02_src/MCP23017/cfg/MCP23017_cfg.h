/// @file MCP23017_cfg.h
/// @brief Contains configuration data used for MCP23017 GPIO expander
/// @author Aleksandra Petrovic

#ifndef MCP23017_CFG_H_
#define MCP23017_CFG_H_

#include "MCP23017.h"

/// Sets all of GPIOA pins as input
#define MCP23017_GPIOA_INPUT 0xFF
/// Sets all of GPIOB pins as output
#define MCP23017_GPIOB_OUTPUT 0x00
/// Enables pull-up on all GPIOA pins
#define MCP23017_GPIOA_PULLUP 0xFF
/// Enables interrupt-on-change on all GPIOA pins
#define MCP23017_GPIOA_INTERRUP_ENABLE 0xFF
/// Sets default value which indicates if interrupt has occurred
#define MCP23017_GPIOA_DEFVAL 0xFF
/// Value used for initial configuration of GPIOA register
#define MCP23017_GPIOA_IOCON 0x68
/// EXTI IMR value for PB1 pin
#define EXTI_IMR_PB1 (1u << 1u)
/// EXTI RTSR value for PB1 pin
#define EXTI_RTSR_PB1 (1u << 1u)
/// EXTI FTSR value for PB1
#define EXTI_FTSR_PB1 (1u << 1u)
/// EXTI PR value set for PB1
#define EXTI_PR_PB1 (1u << 1u)

/// Value that turns all LEDs off
#define LEDS_OFF 0x00
/// Value that turns LED1 on
#define LED1_ON 0x01
/// Value that turns LED2 on
#define LED2_ON 0x02
/// Value that turns LED3 on
#define LED3_ON 0x04
/// Value that turns LED4 on
#define LED4_ON 0x08
/// Value that turns LED5 on
#define LED5_ON 0x10
/// Value that turns LED6 on
#define LED6_ON 0x20
/// Value that turns LED7 on
#define LED7_ON 0x40
/// Value that turns LED8 on
#define LED8_ON 0x80
/// Value of pressed button
#define BUTTON_PRESSED 0x7F
/// Value that defines period for button activation in milliseconds
#define BUTTON_PERIOD 200u
/// Period of the button activations
#define BUTTON_MAINFUNC_PERIOD 10

/// Array of range structures that describes which LED corresponds with which range of bearings
t_LED_Range MCP23017_t_Ranges[] = {
			{ 0,   30,  LED1_ON, b_FALSE },
			{ 330, 360, LED1_ON, b_FALSE },
			{ 15,  75,  LED2_ON, b_FALSE },
			{ 60,  120, LED3_ON, b_FALSE },
			{ 105, 165, LED4_ON, b_FALSE },
			{ 150, 210, LED5_ON, b_FALSE },
			{ 195, 255, LED6_ON, b_FALSE },
			{ 240, 300, LED7_ON, b_FALSE },
			{ 285, 345, LED8_ON, b_FALSE },
};

/// Used to determine the length of MCP23017_t_Ranges array
uint16_t MCP23017_u_Length = sizeof(MCP23017_t_Ranges) / sizeof(MCP23017_t_Ranges[0]);

#endif /* MCP23017_CFG_H_ */
