/// @file UARTM_cfg.h
/// @brief Contains configuration data used for UART serial communication
/// @author Aleksandra Petrovic

#ifndef UARTM_CFG_H_
#define UARTM_CFG_H_

#include "stm32f439xx.h"
#include "stm32f4xx_hal.h"
#include "Registers.h"

/// Defines the size of a temp buffer for messages in UART
#define TEMP_BUFFER (10u)
/// Length of a ring buffer used to receive a message from a GPS
#define BUFFER_LENGTH (512u)
/// Register address used for toggling pin output
#define UARTM_REGISTER_GROUP_GPIOA (GPIOA)
/// Register Group for Universal synchronous  asynchronous Receiver/Transmitter
#define UARTM_USART2_REGISTER_GROUP (USART2)
/// Enable GPIOA CLOCK
#define AHB1ENR_GPIOA_CLOCK (1u << 0u)
/// Enable UART2 CLOCK
#define APB1ENR_UART2_CLOCK (1u << 17u)
/// Enable GPIOA CLOCK
#define AHB1ENR_CLOCK (1u << 0u)
/// Configure Pin PA3 Alternate Function group
#define MODER_PIN_PA3 (2u << 6u)
/// Configure Pin PD5 Alternate Function group
#define MODER_PIN_PD5 (2u << 10u)
/// Configure Speed of Pin PA3
#define OSPEEDR_PIN_PA3 (3u << 6u)
/// Configure Speed of Pin PD5
#define OSPEEDR_PIN_PD5 (3u << 10u)
/// Alternate Function at USART2 Pin PA3
#define AFR_PIN_PA3 (7u << 12u)
/// Alternate Function at USART2 Pin PD5
#define AFR_PIN_PD5 (7u << 20u)
/// Enable USART
#define CR1_USART_ENABLE (1u << 13u)
/// Define word length to 8 bit word
#define CR1_WORD_LENGTH (1u << 12u)
/// Configure baud rate of 115200
#define BRR_BAUD_RATE_USART2 (15u << 0u)
/// Configure PCLK1 Frequency at 45 MHz
#define BRR_PCLK1_USART2 (292u << 4u)
/// Register address used for reset and clock control
#define UARTM_RCC_GROUP (RCC)
/// Register address used for toggling pin output
#define UARTM_REGISTER_GROUP (GPIOD)
/// Register Group for Universal Synchronous Asynchronous Receiver/Transmitter
#define UARTM_USART_REGISTER_GROUP (USART3)
/// Enable UART3 CLOCK
#define APB1ENR_UART3_CLOCK (1u << 18u)
/// Enable GPIOD CLOCK
#define AHB1ENR_GPIOD_CLOCK (1u << 3u)
/// Configure Pin PD8 Alternate Function group
#define MODER_PIN_PD8 (2u << 16u)
/// Configure Pin PD9 Alternate Function group
#define MODER_PIN_PD9 (2u << 18u)
/// Configure Speed of Pin PD8
#define OSPEEDR_PIN_PD8 (3u << 16u)
/// Configure Speed of Pin PD9
#define OSPEEDR_PIN_PD9 (3u <<  18u)
/// Alternate Function at USART2 Pin PD8
#define AFR_PIN_PD8 (7u << 0u)
/// Alternate Function at USART2 Pin PD9
#define AFR_PIN_PD9 (7u << 4u)
/// Enable USART
#define CR1_USART_ENABLE (1u << 13u)
/// Define word length to 8 bit word
#define CR1_WORD_LENGTH (1u << 12u)
/// Configure baud rate of 9600
#define BRR_BAUD_RATE (7u << 0u)
/// Configure PCLK1 Frequency at 45 MHz
#define BRR_PCLK1 (24u << 4u)
/// Enable the Receiver
#define CR1_RECEIVER_ENABLE (1u << 2u)
/// Enable the Transmitter
#define CR1_TRANSMITTER_ENABLE (1u << 3u)
/// Enable Receive Interrupt
#define CR1_RXNEIE_ENABLE (1u << 5u)
/// Enable Transmit Interrupt
#define CR1_TXEIE_ENABLE (1u << 7u)
/// Turn on LED on a pin PA5
#define ODR_LED_ON (1u << 14u)
#endif /* UARTM_CFG_H_ */
