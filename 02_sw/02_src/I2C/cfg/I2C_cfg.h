/// @file I2C_cfg.h
/// @brief Contains configuration data used for I2C communication
/// @author Aleksandra Petrovic

#ifndef I2C_CFG_H_
#define I2C_CFG_H_

#include "Registers.h"

/// Enable GPIOB CLOCK
#define RCC_AHB1ENR_GPIOB_CLOCK (1u << 1u)
/// Enable I2C1 CLOCK
#define RCC_APB1ENR_I2C1_CLOCK (1u << 21u)
/// Configure Pin PB6 Alternate Function group
#define GPIOB_MODER_PIN_PB6 (2u << 12u)
/// Configure Pin PB9 Alternate Function group
#define GPIOB_MODER_PIN_PB9 (2u << 18u)
/// Open drain output for PB6
#define GPIOB_OTYPER_PIN_PB6 (1u << 6u)
/// Open drain output for PB9
#define GPIOB_OTYPER_PIN_PB9 (1u << 9u)
/// Configure Speed of Pin PB6
#define GPIOB_OSPEEDR_PIN_PB6 (3u << 12u)
/// Configure Speed of Pin PB9
#define GPIOB_OSPEEDR_PIN_PB9 (3u << 18u)
/// Configure pull-up for PB6
#define GPIOB_PUPDR_PIN_PB6 (1u << 12u)
/// Configure pull-up for PB9
#define GPIOB_PUPDR_PIN_PB9 (1u << 18u)
/// Alternate function at I2C1 Pin PB6
#define GPIOB_AFR_PIN_PB6 (4u << 24u)
/// Alternate function at I2C1 Pin PB9
#define GPIOB_AFR_PIN_PB9 (4u << 4u)
/// Reset I2C
#define I2C1_CR1_I2C_RESET (1u << 15u)
/// PCLK1 frequency in MHz
#define I2C1_PCLK1_FREQ (45u << 0u)
/// Configure clock control registers
#define I2C1_CCR_CONFIG (225u << 0u)
/// TRISE configuration
#define I2C1_TRISE_CONFIG (46)
/// I2C Enable
#define I2C1_CR1_PE (1u << 0u)
/// Stop I2C
#define I2C1_CR1_STOP (1u << 9u)
/// Generate start for I2C
#define I2C1_CR1_START (1u << 8u)
/// Start bit of I2C1
#define I2C1_SR1_SB (1u << 0u)
/// BERR flag
#define I2C1_SR_BERR (1u << 8u)
/// I2C1 ADDR bit
#define I2C1_SR1_ADDR (1u << 1u)
/// AF bit
#define I2C1_SR1_AF (1u << 10u)
/// Acknowledgment bit
#define I2C1_CR1_ACK (1u << 10u)
/// Data register empty bit
#define I2C1_SR1_TXE (1u << 7u)
/// Byte transfer finish bit
#define I2C1_SR1_BTF (1u << 2u)
/// Data register not empty
#define I2C1_SR1_RXNE (1u << 6u)
/// System configuration controller clock enable
#define RCC_APB2ENR_SYSCFGEN_ENABLE (1u << 14u)
/// GPIOB PB1 set as input
#define GPIOB_PB1_INPUT (3u << 2u)
/// GPIOB PB1 set as pull-up
#define GPIOB_PB1_PULLUP (1u << 2u)
/// EXTI1 line enable
#define EXTI1_EN (1u << 4u)

#endif /* I2C_CFG_H_ */
