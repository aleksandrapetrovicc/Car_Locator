/// @file MONITOR.c
/// @brief Main file used for monitoring number of activations of on boar LED
/// @author Aleksandra Petrovic

#include "MONITOR.h"

/// Counter of deactivations of on board LED
uint8_t numberOfDetections  = 0u;
/// Counter of activations of on board LED
uint8_t numberOfActivations = 0u;

void Task_LED_CP_Start()
{
  numberOfActivations++;
}

void Task_LED_CP_End()
{
  numberOfDetections++;
}
