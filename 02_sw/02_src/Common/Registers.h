/// @file Registers.h
/// @brief Contains configuration registers used to setup UART communication protocol
/// @author Aleksandra Petrovic

#ifndef COMMON_REGISTERS_H_
#define COMMON_REGISTERS_H_

/// USART2 base of APB1 peripheral
#define USART2_BASE (APB1PERIPH_BASE + 0x4400UL)
/// GPIOA base of AHB1 peripheral
#define GPIOA_BASE (AHB1PERIPH_BASE + 0x0000UL)
/// RCC register base
#define RCC_BASE (AHB1PERIPH_BASE + 0x3800UL)
/// USART2->SR register
#define USART2_SR (USART2_BASE + 0x0000UL)
/// USART2->DR register
#define USART2_DR (USART2_BASE + 0x0004UL)
/// USART2->BRR register
#define USART2_BRR (USART2_BASE + 0x0008UL)
/// USART2->CR1 register
#define USART2_CR1 (USART2_BASE + 0x000CUL)
/// GPIOA->MODER register
#define GPIOA_MODER (GPIOA_BASE + 0x0000UL)
/// GPIOA->SPEEDER register
#define GPIOA_OSPEEDER (GPIOA_BASE + 0x0008UL)
/// GPIOA->AFR register
#define GPIOA_AFR0 (GPIOA_BASE + 0x0020UL)
/// Definition of 32-bit registers
#define REG32(address) (*((volatile uint32_t *)address))
/// Definition of 16-bit registers
#define REG16(address) (*((volatile uint16_t *) (address) ))
/// Peripheral base address in the alias region
#define REG_PERIPH_BASE (0x40000000UL)
/// USART2 Clock enable
#define RCC_AHB1ENR (RCC_BASE + 0x0030UL)
/// Advanced Peripheral Bus address
#define RCC_APB1ENR (RCC_BASE + 0x0040UL)
/// Advanced Peripheral Bus address
#define RCC_APB2ENR (RCC_BASE + 0x0044UL)
/// USART3 base of APB1 peripheral
#define USART3_BASE (APB1PERIPH_BASE + 0x4800UL)
/// GPIOD base of AHB1 peripheral
#define GPIOD_BASE (AHB1PERIPH_BASE + 0x0C00UL)
/// GPIOB base of AHB1 peripheral
#define GPIOB_BASE (AHB1PERIPH_BASE + 0x0400UL)
/// USART3->SR register
#define USART3_SR (USART3_BASE + 0x0000UL)
/// USART3->DR register
#define USART3_DR (USART3_BASE + 0x0004UL)
/// USART3->BRR register
#define USART3_BRR (USART3_BASE + 0x0008UL)
/// USART3->CR1 register
#define USART3_CR1 (USART3_BASE + 0x000CUL)
/// GPIOD->MODER register
#define GPIOD_MODER (GPIOD_BASE + 0x0000UL)
/// GPIOD->SPEEDER register
#define GPIOD_OSPEEDER (GPIOD_BASE + 0x0008UL)
/// GPIOD->AFR1 register
#define GPIOD_AFR1 (GPIOD_BASE + 0x0024UL)
/// GPIOB->ODR register
#define GPIOB_ODR (GPIOB_BASE + 0x0014UL)
/// GPIOD->AFR0 register
#define GPIOD_AFR0 (GPIOD_BASE + 0x0020UL)
/// I2C1 base of APB1 peripheral
#define I2C1_BASE (APB1PERIPH_BASE + 0x5400UL)
/// GPIOB base address
#define GPIOB_BASE (AHB1PERIPH_BASE + 0x0400UL)
/// GPIOB->MODER register
#define GPIOB_MODER (GPIOB_BASE + 0x0000UL)
/// GPIOB->OTYPER register
#define GPIOB_OTYPER (GPIOB_BASE + 0x0004UL)
/// GPIOB->OSPEEDR register
#define GPIOB_OSPEEDR (GPIOB_BASE + 0x0008UL)
/// GPIOB->PUPDR register
#define GPIOB_PUPDR (GPIOB_BASE + 0x000CUL)
/// GPIOB->AFR0 register
#define GPIOB_AFR0 (GPIOB_BASE + 0x0020UL)
/// GPIOB->AFR1 register
#define GPIOB_AFR1 (GPIOB_BASE + 0x0024UL)
/// I2C1->CR1 register
#define I2C1_CR1 (I2C1_BASE + 0x0000UL)
/// I2C1->CR2 register
#define I2C1_CR2 (I2C1_BASE + 0x0004UL)
/// I2C1->CCR register
#define  I2C1_CCR (I2C1_BASE + 0x001CUL)
/// I2C1->TRISE register
#define I2C1_TRISE (I2C1_BASE + 0x0020UL)
/// I2C1->SR1 register
#define I2C1_SR1 (I2C1_BASE + 0x0014UL)
/// I2C1->DR register
#define I2C1_DR (I2C1_BASE + 0x0010UL)
/// I2C1->SR2 register
#define I2C1_SR2 (I2C1_BASE + 0x0018UL)
/// Base address for independent watchdog timer
#define IWDG_BASE (APB1PERIPH_BASE + 0x3000UL)
/// IWDG->KR register
#define IWDG_KR (IWDG_BASE + 0x0000UL)
/// IWDG->PR register
#define IWDG_PR (IWDG_BASE + 0x0004UL)
/// IWDG->RLR register
#define IWDG_RLR (IWDG_BASE + 0x0008UL)
/// IWDG->SR register
#define IWDG_SR (IWDG_BASE + 0x000CUL)
/// EXTI->IMR register
#define EXTI_IMR (EXTI_BASE + 0x0000UL)
/// EXTI->EMR register
#define EXTI_EMR (EXTI_BASE + 0x0004UL)
/// EXTI->RTSR register
#define EXTI_RTSR (EXTI_BASE + 0x0008UL)
/// EXTI->FTSR register
#define EXTI_FTSR (EXTI_BASE + 0x000CUL)
/// EXTI->SWIER register
#define EXTI_SWIER (EXTI_BASE + 0x0010UL)
/// EXTI->PR register
#define EXTI_PR (EXTI_BASE + 0x0014UL)

//#else
//#error "Platform configuration not defined!"
//#endif /* PLATFORM_CONFIG_STM32F4 */
#endif /* COMMON_REGISTERS_H_ */
