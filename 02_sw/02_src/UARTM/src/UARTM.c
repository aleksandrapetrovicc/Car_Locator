/// @file UARTM.c
/// @brief Main file used to send messages through UART serial communication
/// @author Aleksandra Petrovic

#include "UARTM.h"
#include "MSGM.h"
#include <stdio.h>
#include <string.h>
#include "tim.h"

uint16_t       UARTM_u_index                  = 0u;
static uint8_t UARTM_a_array[TEMP_BUFFER]     = {0u};
uint8_t        UARTM_a_provera[BUFFER_LENGTH] = {0u};
uint8_t        UARTM_u_data[BUFFER_LENGTH]    = {0u};
static uint32_t       start_address;
static uint32_t       max_delay = 300u;
static uint32_t       max_time = 65536u;
static uint32_t       delayed_value;
static uint32_t       delayed_overflow_value;

void UARTM_v_Uart3Config()
{
  // 1. Enable the UART CLOCK and GPIO CLOCK
  REG32(RCC_APB1ENR) |= APB1ENR_UART3_CLOCK;                       // Enable UART3 CLOCK
  REG32(RCC_AHB1ENR) |= AHB1ENR_GPIOD_CLOCK;                       // Enable GPIOD CLOCK

  // 2. Configure the UART PINs for Alternate Functions
  REG32(GPIOD_MODER) |= MODER_PIN_PD8;                             // Bits (17:16) = 1:0 --> Alternate Function for Pin PD8
  REG32(GPIOD_MODER) |= MODER_PIN_PD9;                             // Bits (19:18) = 1:0 --> Alternate Function for Pin PD9

  REG32(GPIOD_OSPEEDER) |= OSPEEDR_PIN_PD8 | OSPEEDR_PIN_PD9;      // Bits (17:16) = 1:1 and Bits (19:18) = 1:1 --> High Speed for PIN PD8 and PD9

  REG32(GPIOD_AFR1) |= AFR_PIN_PD8;                                // Bytes (3:2:1:0) = 0:1:1:1 --> AF7 Alternate Function for USART2 at Pin PD8
  REG32(GPIOD_AFR1) |= AFR_PIN_PD9;                                // Bytes (7:6:5:4) = 0:1:1:1 --> AF7 Alternate function for USART2 at Pin PD9

  // 3. Enable the USART by writing the UE bit in USART_CR1 register to 1.
  REG32(USART3_CR1) = 0x00u;                                       // clear all
  REG32(USART3_CR1) |= CR1_USART_ENABLE;                           // UE = 1... Enable USART

  // 4. Program the M bit in USART_CR1 to define the word length.
  REG32(USART3_CR1) &= ~(CR1_WORD_LENGTH);                         // M = 0; 8 bit word length

  // 5. Select the desired baud rate using the USAR_BRR register.
  REG32(USART3_BRR) = (BRR_BAUD_RATE) | (BRR_PCLK1);               // Baud rate of 115200, PCLK1 at 45Mhz

  // 6. Enable the Transmitter/Receiver by Settin1g the TE and RE bits in USART_CR1 Register
  REG32(USART3_CR1) |= CR1_RECEIVER_ENABLE;                        // RE=1... Enable the Receiver
  REG32(USART3_CR1) |= CR1_TRANSMITTER_ENABLE;                     // TE=1... Enable the Receiver

//   7. Enable Interrupt routine for receiving
  REG32(USART2_CR1) |= CR1_RXNEIE_ENABLE;                          // Enable RX interrupt
  REG32(USART3_CR1) |= CR1_RXNEIE_ENABLE;                          // Enable RX interrupt
  NVIC_EnableIRQ(USART2_IRQn);                                     // Enable Global interrupt for USART2
  NVIC_EnableIRQ(USART3_IRQn);                                     // Enable Global interrupt for USART3
}

void UARTM_v_Uart2Config()
{
  // 1. Enable the UART CLOCK and GPIO CLOCK
  REG32(RCC_APB1ENR) |= APB1ENR_UART2_CLOCK;                       // Enable UART2 CLOCK
  REG32(RCC_AHB1ENR) |= AHB1ENR_GPIOD_CLOCK;                       // Enable GPIOD CLOCK
  REG32(RCC_AHB1ENR) |= AHB1ENR_GPIOA_CLOCK;                       // Enable GPIOA CLOCK

  // 2. Configure the UART PINs for Alternate Functions
  REG32(GPIOA_MODER) |= MODER_PIN_PA3;                             // Bits (7:6) = 1:0 --> Alternate Function for Pin PA3
  REG32(GPIOD_MODER) |= MODER_PIN_PD5;                             // Bits (11:10) = 1:0 --> Alternate Function for Pin PD5

  REG32(GPIOA_OSPEEDER) |= OSPEEDR_PIN_PA3;                        // Bits (7:6) = 1:1 and Bits (11:10) = 1:1 --> High Speed for PIN PA3 and PD5
  REG32(GPIOD_OSPEEDER) |= OSPEEDR_PIN_PD5;

  REG32(GPIOA_AFR0) |= AFR_PIN_PA3;                                // Bytes (15:14:13:12) = 0:1:1:1 --> AF7 Alternate Function for USART2 at Pin PA3
  REG32(GPIOD_AFR0) |= AFR_PIN_PD5;                                // Bytes (23:22:21:20) = 0:1:1:1 --> AF7 Alternate function for USART2 at Pin PD5

  // 3. Enable the USART by writing the UE bit in USART_CR1 register to 1.
  REG32(USART2_CR1) = 0x00u;                                       // clear all
  REG32(USART2_CR1) |= CR1_USART_ENABLE;                           // UE = 1... Enable USART

  // 4. Program the M bit in USART_CR1 to define the word length.
  REG32(USART2_CR1) &= ~(CR1_WORD_LENGTH);                         // M = 0; 8 bit word length

  // 5. Select the desired baud rate using the USAR_BRR register.
  REG32(USART2_BRR) = (BRR_BAUD_RATE_USART2) | (BRR_PCLK1_USART2); // Baud rate of 115200, PCLK1 at 45Mhz

  // 6. Enable the Transmitter/Receiver by Settin1g the TE and RE bits in USART_CR1 Register
  REG32(USART2_CR1) |= CR1_RECEIVER_ENABLE;                        // RE=1... Enable the Receiver
  REG32(USART2_CR1) |= CR1_TRANSMITTER_ENABLE;                     // TE=1... Enable the Receiver

  // 7. Enable Interrupt routine for receiving
  REG32(USART2_CR1) |= CR1_RXNEIE_ENABLE;                          // Enable RX interrupt
  NVIC_EnableIRQ(USART2_IRQn);                                     // Enable Global interrupt for USART2
}

void UARTM2_v_SendChar(uint8_t u_character)
{
	REG32(USART2_DR) = u_character;                                  // Load the data into DR register
	while (!(REG32(USART2_SR) & (USART_SR_TC)))
	{
                                                                   // Wait for TC to SET.. This indicates that the data has been transmitted
	}
}

void UARTM3_v_SendChar(uint8_t u_character)
{
	REG32(USART3_DR) = u_character;                                  // Load the data into DR register
	while (!(REG32(USART3_SR) & (USART_SR_TC)))
	{
                                                                   // Wait for TC to SET.. This indicates that the data has been transmitted
	}
}

void UARTM2_v_SendString(uint8_t *u_string)
{
  while (*u_string != 0)                                           // Move through every character until null character is reached
  {
    UARTM2_v_SendChar(*u_string);                                   // Send character by character calling UARTM_v_SendChar function
    u_string++;                                                    // Increment u_string parameter until end of string is reached
  }
}

void UARTM3_v_SendString(uint8_t *u_string)
{
  while (*u_string != 0)                                           // Move through every character until null character is reached
  {
    UARTM3_v_SendChar(*u_string);                                   // Send character by character calling UARTM_v_SendChar function
    u_string++;                                                    // Increment u_string parameter until end of string is reached
  }
}

uint8_t UARTM2_u_GetChar()
{
	start_address = __HAL_TIM_GET_COUNTER(&htim10);
	delayed_value = start_address + max_delay;
	delayed_overflow_value = max_time - start_address + max_delay;

	uint8_t u_temp;
  	if (start_address <= max_time - max_delay)
  	{
  		while (!((REG32(USART2_SR) & USART_SR_RXNE) || (__HAL_TIM_GET_COUNTER(&htim10) >= delayed_value)))
  		{
                                                                   // Wait until the character is ready in USART3 buffer
  		}
    }
  else
  {
	  while (!((REG32(USART2_SR) & USART_SR_RXNE) || (__HAL_TIM_GET_COUNTER(&htim10) >= delayed_overflow_value)))

	  {
	                                                                    // Wait until the character is ready in USART3 buffer
	  }
  }
  u_temp = (uint8_t) REG32(USART2_DR);                             // Transfer data from DR register to temporary variable
  return u_temp;                                                   // Return the content of a register
}

uint8_t UARTM3_u_GetChar()
{
	start_address = __HAL_TIM_GET_COUNTER(&htim10);
	delayed_value = start_address + max_delay;
	delayed_overflow_value = max_time - start_address + max_delay;

	uint8_t u_temp;

  	  if (start_address <= max_time - max_delay)
  	  {
  		  while (!((REG32(USART3_SR) & USART_SR_RXNE) || (__HAL_TIM_GET_COUNTER(&htim10) >= delayed_value)))
   	  {
                                                                    // Wait until the character is ready in USART3 buffer
   	  }
     }
  	  else
  	  {
  		  while (!((REG32(USART3_SR) & USART_SR_RXNE) || (__HAL_TIM_GET_COUNTER(&htim10) >= delayed_overflow_value)))
  		  {
 	                                                                    // Wait until the character is ready in USART3 buffer
  		  }
   }

  u_temp = (uint8_t) REG32(USART3_DR);                             // Transfer data from DR register to temporary variable
  return u_temp;                                                   // Return the content of a register
}

void USART3_IRQHandler(void)
{
  // Check if interrupt happened because of RXNEIE register
  if (REG32(USART3_SR) & USART_SR_RXNE)                             // If RX register is not empty
  {
    start_address = __HAL_TIM_GET_COUNTER(&htim10);
    delayed_value = start_address + max_delay;
    delayed_overflow_value = max_time - start_address + max_delay;
    uint8_t u_temp = (uint8_t)REG32(USART3_DR);                             // Fetch the data received
   if (start_address <= max_time - max_delay)
   {
    while (!((REG32(USART3_SR) & USART_SR_TXE) || (__HAL_TIM_GET_COUNTER(&htim10) >= delayed_value)));
   }
   else
   {
     while (!((REG32(USART3_SR) & USART_SR_TXE) || (__HAL_TIM_GET_COUNTER(&htim10) >= delayed_overflow_value)));
   }
    MSGM_u_CircularBufferPush(RING_BUFFER1, u_temp);
  }
}

void USART2_IRQHandler()
{
  start_address = __HAL_TIM_GET_COUNTER(&htim10);
  delayed_value = start_address + max_delay;
  delayed_overflow_value = max_time - start_address + max_delay;
  // Check if interrupt happened because of RXNEIE register
  if (REG32(USART2_SR) & USART_SR_RXNE)                             // If RX register is not empty
  {
    uint8_t u_temp = (uint8_t)REG32(USART2_DR);                    // Fetch the data received from USART2

    if (start_address <= max_time - max_delay)
    {
     while (!((REG32(USART2_SR) & USART_SR_TXE) || (__HAL_TIM_GET_COUNTER(&htim10) >= delayed_value)));
    }
    else
    {
      while (!((REG32(USART2_SR) & USART_SR_TXE) || (__HAL_TIM_GET_COUNTER(&htim10) >= delayed_overflow_value)));
    }
    REG32(USART2_DR) = u_temp;
    REG32(USART3_DR) = u_temp;                                      // Send the data temporary variable to USART3
    MSGM_u_CircularBufferPush(RING_BUFFER1, u_temp);                // Push the data to the ring buffer for storage
  }
}

void UARTM_v_ClearBuffer()
{
  uint8_t u_Cnt;

  for (u_Cnt = 0u; u_Cnt < UARTM_u_index; u_Cnt++)
  {
    UARTM_a_array[u_Cnt] = 0u;  // Clear the data of a global array
  }
  UARTM_u_index = 0u;
}
