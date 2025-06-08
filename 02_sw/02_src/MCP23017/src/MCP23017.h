/// @file MCP23017.h
/// @brief Header file used for manipulating different operations with MCP23017 GPIO expander
/// @author Aleksandra Petrovic

#ifndef MCP23017_H_
#define MCP23017_H_

#include "freertos.h"
#include "cmsis_os.h"
#include "I2C.h"
#include "CALCM.h"

/// Address of MCP23017 GPIO expander (slave address)
#define MCP23017_ADDRESS 0x20

/// IODIRA register address
#define MCP23017_IODIRA 0x00
/// IODIRB register address
#define MCP23017_IODIRB 0x01
/// GPIOA register address
#define MCP23017_GPIOA 0x12
/// GPIOB register address
#define MCP23017_GPIOB 0x13
/// GPPUA register address (controls the pull-up resistors for the port pins)
#define MCP23017_GPPUA 0x0C
/// IPOLA register address (input polarity register)
#define MCP23017_IPOLA 0x02
/// IPOLB register address (input polarity register)
#define MCP23017_IPOLB 0x03
/// Interrupt on change control register
#define MCP23017_GPINTENA 0x04
/// Default compare register for interrupt on change
#define MCP23017_DEFVALA 0x06
/// Interrupt control register
#define MCP23017_INTCONA 0x08
/// IOCON register address
#define MCP23017_IOCONA 0x0A
/// Value for MCP23017_u_delay
#define MCP23017_DELAY 400u
/// Value of pressed button
#define BUTTON_PRESSED 0x7F

/// This structure is used to connect proper LED with its range of coordinates
typedef struct {
  uint16_t u_Low;  		///< Field used to receive a low limit for bearing
  uint16_t u_High;  	///< Field used to receive a high limit for bearing
  uint8_t u_LED;    	///< Field used to receive proper LED
  boolean b_TurnedON;	///< Field used for flag that indicates if the LED should be turned on
} t_LED_Range;

/// @brief Function used for configuring EXTI interrupt on line 1
///
/// @pre MCP23017 GPIO expander must be configured
/// @post When interrupt condition occurs, interrupt will be generated
/// @param None
///
/// @return None
///
/// @globals None
///
/// @InOutCorelation Function sets up registers for EXTI interrupt on line 1.
/// @callsequence
///   @startuml "MCP23017_v_EXTI1_Configuration.png"
///     title "Sequence diagram for function MCP23017_v_EXTI1_Configuration"
///     -> MCP23017: MCP23017_v_EXTI1_Configuration()
///     MCP23017++
///       rnote over MCP23017: Registers configured for NVIC external interrupt.
///     <- MCP23017
///     MCP23017--
///   @enduml

void MCP23017_v_EXTI1_Configuration(void);

/// @brief Function used for configuring MCP23017 GPIO expander
///
/// @pre I2C must be configured
/// @post MCP23017 GPIO expander is set for read and write operations
/// @param None
///
/// @return None
///
/// @globals None
///
/// @InOutCorelation Function sets up registers for MCP23017 GPIO expander.
/// @callsequence
///   @startuml "MCP23017_v_Init.png"
///     title "Sequence diagram for function MCP23017_v_Init"
///     -> MCP23017: MCP23017_v_Init()
///     MCP23017++
///       MCP23017 -> MCP23017: v_Write(MCP23017_ADDRESS << 1, MCP23017_IODIRA, MCP23017_GPIOA_INPUT)
///       MCP23017 -> MCP23017: v_Write(MCP23017_ADDRESS << 1, MCP23017_GPPUA, MCP23017_GPIOA_PULLUP)
///       MCP23017 -> MCP23017: v_Write(MCP23017_ADDRESS << 1, MCP23017_IODIRB, MCP23017_GPIOB_OUTPUT)
///       MCP23017 -> MCP23017: v_Write(MCP23017_ADDRESS << 1, MCP23017_IOCONA, MCP23017_GPIOA_IOCON)
///       MCP23017 -> MCP23017: v_Write(MCP23017_ADDRESS << 1, MCP23017_GPINTENA, MCP23017_GPIOA_INTERRUP_ENABLE)
///       MCP23017 -> MCP23017: v_Write(MCP23017_ADDRESS << 1, MCP23017_INTCONA, MCP23017_GPIOA_INTERRUP_ENABLE)
///       MCP23017 -> MCP23017: v_Write(MCP23017_ADDRESS << 1, MCP23017_DEFVALA, MCP23017_GPIOA_DEFVAL)
///       rnote over MCP23017: MCP23017 register configured for input and output and interrupt configuration is done via v_Write() function.
///     <- MCP23017
///     MCP23017--
///   @enduml

void MCP23017_v_Init(void);

/// @brief Function used for turning on the LED based on read coordinates
///
/// @pre MCP23017 GPIO expander must be configured
/// @post None
/// @param None
///
/// @return None
///
/// @globals None
///
/// @InOutCorelation Function reads the button state and based on coordinates turns the correct LED on.
/// @callsequence
///   @startuml "MCP23017_v_TurnLEDviaCoordinates.png"
///     title "Sequence diagram for function MCP23017_v_TurnLEDviaCoordinates"
///     -> MCP23017: MCP23017_v_TurnLEDviaCoordinates()
///     MCP23017++
///       MCP23017 -> MCP23017: u_Button_Read()
///       SIM -> MCP23017: SIM_t_Flag()
///       opt if button is pressed
///         rnote over MCP23017: Set b_PressedButton flag
///       else else
///         rnote over MCP23017: Reset b_PressedButton flag
///       end
///       opt if t_flag -> e_CurrentFunction is ReadMessage
///         CALCM -> MCP23017:  CALCM_u_CalculateBearing()
///         loop Goes through LED structure
///           opt if bearing is inside the range
///             rnote over MCP23017: Sets a flag and its LED value is added to local variable u_LEDs in order to turn all correct LEDs on.
///             MCP23017 -> MCP23017: v_TurnLED(u_LEDs)
///         else else
///           rnote over MCP23017: Resets a flag which indicates if the LED is turned on
///         end
///         rnote over MCP23017: If bearing is not inside the range, its flag is set to false value.
///         end
///       else else
///         rnote over MCP23017: Sets t_flag -> e_CurrentFunction as IdleFunction
///       end
///     MCP23017--
///     <- MCP23017
///   @enduml

void MCP23017_v_TurnLEDviaCoordinates(void);
#endif /* MCP23017_H_ */
