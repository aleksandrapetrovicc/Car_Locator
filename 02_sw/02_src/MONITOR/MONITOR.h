/// @file MONITOR.h
/// @brief Header file used for monitoring number of activations of on boar LED
/// @author Aleksandra Petrovic

#ifndef MONITOR_H_
#define MONITOR_H_

#include "stm32f439xx.h"

/// @brief Function used for monitoring on board LED
///
/// @pre None
/// @post None
/// @param None
///
/// @return None
///
/// @globals uint8_t numberOfActivations
///
/// @InOutCorelation Function counts how many times did the on board LED turn on.
/// @callsequence
///   @startuml "Task_LED_CP_Start.png"
///     title "Sequence diagram for function Task_LED_CP_Start"
///     -> MONITOR: Task_LED_CP_Start()
///     MONITOR++
///     rnote over MONITOR: Increases numberOfActivations when entering the function.
///     MONITOR--
///     <- MONITOR
///   @enduml

void Task_LED_CP_Start(void);

/// @brief Function used for monitoring on board LED
///
/// @pre None
/// @post None
/// @param None
///
/// @return None
///
/// @globals uint8_t numberOfDetections
///
/// @InOutCorelation Function counts how many times did the on board LED turn off.
/// @callsequence
///   @startuml "Task_LED_CP_End.png"
///     title "Sequence diagram for function Task_LED_CP_End"
///     -> MONITOR: Task_LED_CP_End()
///     MONITOR++
///     rnote over MONITOR: Increments numberOfDetections when entering function.
///     <- MONITOR
///     MONITOR--
///   @enduml

void Task_LED_CP_End(void);






/// @brief Function used for monitoring on board LED
///
/// @pre None
/// @post None
/// @param None
///
/// @return None
///
/// @globals uint8_t numberOfDetections
///
/// @InOutCorelation Function counts how many times did the on board LED turn off.
/// @callsequence
///   @startuml "Task_LED_CP_End.png"
///     title "Sequence diagram for function Task_LED_CP_End"
///     TSK_Idle++
///       rnote over TSK_Idle: Other tasks are not active.
///     TSK_Idle--
///     TSK_Led++
///       rnote over TSK_Led: Blink LED every 500 ms.
///       -> TSK_Led: LEDM_v_Main()
///     TSK_Led--
///     TSK_Com++
///       rnote over TSK_Com: Processes messages from GPS module.
///       -> TSK_Com: MSGM_v_StateMachine()
///     TSK_Com--
///     TSK_SIM++
///       rnote over TSK_SIM: Checks current and sets next function for SIM module.
///       -> TSK_SIM: SIM_v_StateMachine()
///     TSK_SIM--
///     TSK_MCP23017++
///       rnote over TSK_MCP23017: If button is pressed, sets next SIM function to MakeCall and if SIM function is ReadMessage, calculates direction and based on the value, turns the LEDs on.
///       -> TSK_MCP23017: MCP23017_v_TurnLEDviaCoordinates()
///     TSK_MCP23017--
///   @enduml

#endif /* MONITOR_H_ */
