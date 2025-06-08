/// @file I2C.c
/// @brief Main file used for I2C communication
/// @author Aleksandra Petrovic

#include "I2C.h"
#include "tim.h"

// Variable used for storing timer value when entering the function
static uint32_t u_StartAddress = 0u;
// Variable used for exiting function if max time has passed
static uint32_t u_DelayedValue = 0u;
// Variable used for exiting function if max time has passed and timer max value overflowed
static uint32_t u_DelayedOverflowValue = 0u;

void I2C_v_Configure()
{
  // 1. Enable the I2C clock and GPIO clock
  // Enable I2C clock
  REG32(RCC_APB1ENR) |= RCC_APB1ENR_I2C1_CLOCK;
  // Enable GPIOB clock
  REG32(RCC_AHB1ENR) |= RCC_AHB1ENR_GPIOB_CLOCK;

  // 2.a) Select Alternate function in MODER register
  // Bits (13:12)= 1:0 --> Alternate function for pin PB6; Bits (19:18)= 1:0 --> Alternate function for pin PB9
  REG32(GPIOB_MODER) |= GPIOB_MODER_PIN_PB6 | GPIOB_MODER_PIN_PB9;
  // 2.b) Select open drain output
  // Bit6=1, Bit9=1  output open drain
  REG32(GPIOB_OTYPER) |= GPIOB_OTYPER_PIN_PB6 | GPIOB_OTYPER_PIN_PB9;
  // 2.c) Select high speed for the pins
  // Bits (13:12)= 1:1 --> High speed for pin PB8; Bits (19:18)= 1:1 --> High speed for pin PB9
  REG32(GPIOB_OSPEEDR) |= GPIOB_OSPEEDR_PIN_PB6 | GPIOB_OSPEEDR_PIN_PB9;
  // 2.d) Select pull-up for both the pins
  // Bits (17:16)= 0:1 --> Pull up for pin PB8; Bits (19:18)= 0:1 --> pull up for pin PB9
  REG32(GPIOB_PUPDR) |= GPIOB_PUPDR_PIN_PB6 | GPIOB_PUPDR_PIN_PB9;
  // 2.e) Configure the Alternate function in AFR register
  // AF4 for pin PB6
  REG32(GPIOB_AFR0) |= GPIOB_AFR_PIN_PB6;
  // Bits (7:6:5:4) = 0:1:0:0 --> AF4 for pin PB9
  REG32(GPIOB_AFR1) |= GPIOB_AFR_PIN_PB9;

  // 3. Reset the I2C
  // Reset the I2C
  REG32(I2C1_CR1) |= I2C1_CR1_I2C_RESET;
  // Normal operation
  REG32(I2C1_CR1) &= ~I2C1_CR1_I2C_RESET;

  // 4. Program the peripheral input clock in I2C_CR2 register in order to generate correct timings
  // PCLK1 frequency in MHz
  REG32(I2C1_CR2) |= I2C1_PCLK1_FREQ;

  // 5. Configure the clock control registers
 REG32(I2C1_CCR) = I2C1_CCR_CONFIG;

  // 6. Configure the rise time register
  REG32(I2C1_TRISE) = I2C1_TRISE_CONFIG;

  // 7. Program the I2C_CR1 register to enable the peripheral
  // Enable I2C
  REG32(I2C1_CR1) |= I2C1_CR1_PE;
}

void I2C_v_Start()
{
  // Generate start condition
  REG32(I2C1_CR1) |= I2C1_CR1_START;

  u_StartAddress = __HAL_TIM_GET_COUNTER(&htim10);
  u_DelayedValue = u_StartAddress + I2C_MAX_DELAY;
  u_DelayedOverflowValue = TIMER_MAX_TIME - u_StartAddress + I2C_MAX_DELAY;

  if(u_StartAddress <= TIMER_MAX_TIME - I2C_MAX_DELAY)
  {
	// Wait for SB (start bit) bit to set or max time to pass
    while (!((REG32(I2C1_SR1) & I2C1_SR1_SB) || (__HAL_TIM_GET_COUNTER(&htim10) >= u_DelayedValue)));
  }
  else
  {
	// Wait for SB (start bit) bit to set or max time to pass
    while (!((REG32(I2C1_SR1) & I2C1_SR1_SB) || (__HAL_TIM_GET_COUNTER(&htim10) >= u_DelayedOverflowValue)));
  }
}

void I2C_v_Stop()
{
  // Stop I2C
  REG32(I2C1_CR1) |= I2C1_CR1_STOP;
}

uint8_t I2C_u_SendAddress(uint8_t u_Address)
{
  // Send the address
  REG32(I2C1_DR) = u_Address;
  // Clear the BERR flag
  REG32(I2C1_SR1) &= ~I2C1_SR_BERR;

  u_StartAddress = __HAL_TIM_GET_COUNTER(&htim10);
  u_DelayedValue = u_StartAddress + I2C_MAX_DELAY;
  u_DelayedOverflowValue = TIMER_MAX_TIME - u_StartAddress + I2C_MAX_DELAY;

  if(u_StartAddress <= TIMER_MAX_TIME - I2C_MAX_DELAY)
  {
	// Wait for ADDR bit to set or max time to pass
    while (!((REG32(I2C1_SR1) & I2C1_SR1_ADDR) || (__HAL_TIM_GET_COUNTER(&htim10) >= u_DelayedValue)));
  }
  else
  {
	// Wait for ADDR bit to set or max time to pass
    while (!((REG32(I2C1_SR1) & I2C1_SR1_ADDR) || (__HAL_TIM_GET_COUNTER(&htim10) >= u_DelayedOverflowValue)));
  }
  // Clear the AF bit if it sets
  REG32(I2C1_SR1) &= ~I2C1_SR1_AF;
  // Read SR1 and SR2 to clear the ADDR bit
  uint8_t u_Temp = REG32(I2C1_SR1) | REG32(I2C1_SR2);
  // Enable the ACK
  REG32(I2C1_CR1) |= I2C1_CR1_ACK;
  return u_Temp;
}

void I2C_v_Write(uint8_t u_Data)
{
  u_StartAddress = __HAL_TIM_GET_COUNTER(&htim10);
  u_DelayedValue = u_StartAddress + I2C_MAX_DELAY;
  u_DelayedOverflowValue = TIMER_MAX_TIME - u_StartAddress + I2C_MAX_DELAY;

  if(u_StartAddress <= TIMER_MAX_TIME - I2C_MAX_DELAY)
  {
	// Wait for TXE bit to set or max time to pass
    while (!((REG32(I2C1_SR1) & I2C1_SR1_TXE) || (__HAL_TIM_GET_COUNTER(&htim10) >= u_DelayedValue)));
  }
  else
  {
	// Wait for TXE bit to set or max time to pass
    while (!((REG32(I2C1_SR1) & I2C1_SR1_TXE) || (__HAL_TIM_GET_COUNTER(&htim10) >= u_DelayedOverflowValue)));
  }
  // Clear the AF bit if it sets
  REG32(I2C1_SR1) &= ~I2C1_SR1_AF;
  REG32(I2C1_DR) = u_Data;
  // Clear the BERR flag
  REG32(I2C1_SR1) &= ~I2C1_SR_BERR;

  u_StartAddress = __HAL_TIM_GET_COUNTER(&htim10);
  u_DelayedValue = u_StartAddress + I2C_MAX_DELAY;
  u_DelayedOverflowValue = TIMER_MAX_TIME - u_StartAddress + I2C_MAX_DELAY;

  if(u_StartAddress <= TIMER_MAX_TIME - I2C_MAX_DELAY)
  {
	// Wait for BTF(byte transfer finished) bit to set or max time to pass
    while (!((REG32(I2C1_SR1) & I2C1_SR1_BTF) || (__HAL_TIM_GET_COUNTER(&htim10) >= u_DelayedValue)));
  }
  else
  {
	// Wait for BTF(byte transfer finished) bit to set or max time to pass
    while (!((REG32(I2C1_SR1) & I2C1_SR1_BTF) || (__HAL_TIM_GET_COUNTER(&htim10) >= u_DelayedOverflowValue)));
  }
  // Clear the AF bit if it sets
  REG32(I2C1_SR1) &= ~I2C1_SR1_AF;
}

uint8_t I2C_u_Read(uint8_t u_Address, uint8_t *u_Buffer, uint8_t u_Size)
{
  uint8_t u_Remaining = u_Size;
  uint8_t u_Temp;
  // 1. If only 1 byte needs to be read
  if(u_Size == 1)
  {
    // 1.a) Write the slave address, and wait for the ADDR bit (bit 1 in SR1) to be set
    // Send the address
    REG32(I2C1_DR) = u_Address;
    // Clear the BERR flag
    REG32(I2C1_SR1) &= ~I2C1_SR_BERR;

    u_StartAddress = __HAL_TIM_GET_COUNTER(&htim10);
    u_DelayedValue = u_StartAddress + I2C_MAX_DELAY;
    u_DelayedOverflowValue = TIMER_MAX_TIME - u_StartAddress + I2C_MAX_DELAY;

    if(u_StartAddress <= TIMER_MAX_TIME - I2C_MAX_DELAY)
    {
      // Wait for ADDR bit to set or max time to pass
      while(!((REG32(I2C1_SR1) & I2C1_SR1_ADDR) || (__HAL_TIM_GET_COUNTER(&htim10) >= u_DelayedValue)));
    }
    else
    {
      // Wait for ADDR bit to set or max time to pass
      while(!((REG32(I2C1_SR1) & I2C1_SR1_ADDR) || (__HAL_TIM_GET_COUNTER(&htim10) >= u_DelayedOverflowValue)));
    }
    // Clear the AF bit if it sets
    REG32(I2C1_SR1) &= ~I2C1_SR1_AF;

    // 1.b) the Acknowledge disable is made during EV6 (before ADDR flag is cleared) and the stop condition generation is made after EV6
    // Clear the ACK bit
    REG32(I2C1_CR1) &= ~I2C1_CR1_ACK;
    // Read SR1 and SR2 to clear the ADDR bit.... EV6 condition
    u_Temp = REG32(I2C1_SR1) | REG32(I2C1_SR2);
    // Stop I2C
    REG32(I2C1_CR1) |= I2C1_CR1_STOP;

    // 1.c) Wait for the RxNE (receive buffer not empty) bit to set
    u_StartAddress = __HAL_TIM_GET_COUNTER(&htim10);
    u_DelayedValue = u_StartAddress + I2C_MAX_DELAY;
    u_DelayedOverflowValue = TIMER_MAX_TIME - u_StartAddress + I2C_MAX_DELAY;

    if(u_StartAddress <= TIMER_MAX_TIME - I2C_MAX_DELAY)
    {
      // Wait for RxNE to set or max time to pass
      while(!((REG32(I2C1_SR1) & I2C1_SR1_RXNE) || (__HAL_TIM_GET_COUNTER(&htim10) >= u_DelayedValue)));
    }
    else
    {
      // Wait for RxNE to set or max time to pass
      while(!((REG32(I2C1_SR1) & I2C1_SR1_RXNE) || (__HAL_TIM_GET_COUNTER(&htim10) >= u_DelayedOverflowValue)));
    }
    // Clear the AF bit if it sets
    REG32(I2C1_SR1) &= ~I2C1_SR1_AF;

    // 1.d) Read the data from the DR
    // Read the data from the data register
    u_Buffer[u_Size - u_Remaining] = REG32(I2C1_DR);
    // Clear the BERR flag
    REG32(I2C1_SR1) &= ~I2C1_SR_BERR;
  }
  // 2. If multiple bytes needs to be read
  else
  {
    // 2.a) Write the slave address, and wait for the ADDR bit (bit 1 in SR1) to be set
    // Send the address
    REG32(I2C1_DR) = u_Address;
    // Clear the BERR flag
    REG32(I2C1_SR1) &= ~I2C1_SR_BERR;

    u_StartAddress = __HAL_TIM_GET_COUNTER(&htim10);
    u_DelayedValue = u_StartAddress + I2C_MAX_DELAY;
    u_DelayedOverflowValue = TIMER_MAX_TIME - u_StartAddress + I2C_MAX_DELAY;

    if(u_StartAddress <= TIMER_MAX_TIME - I2C_MAX_DELAY)
    {
      // Wait for ADDR bit to set or max time to pass
      while(!((REG32(I2C1_SR1) & I2C1_SR1_ADDR) || (__HAL_TIM_GET_COUNTER(&htim10) >= u_DelayedValue)));
    }
    else
    {
      // Wait for ADDR bit to set or max time to pass
      while(!((REG32(I2C1_SR1) & I2C1_SR1_ADDR) || (__HAL_TIM_GET_COUNTER(&htim10) >= u_DelayedOverflowValue)));
    }
    // Clear the AF bit if it sets
    REG32(I2C1_SR1) &= ~I2C1_SR1_AF;

    //2. b) Clear the ADDR bit by reading the SR1 and SR2 Registers
    REG32(I2C1_CR1) &= I2C1_CR1_ACK;
    // Read SR1 and SR2 to clear the ADDR bit
    u_Temp = REG32(I2C1_SR1) | REG32(I2C1_SR2);

    while(u_Remaining > 2)
    {
      // 2.c) Wait for the RXNE (receive buffer not empty) bit to set
      u_StartAddress = __HAL_TIM_GET_COUNTER(&htim10);
      u_DelayedValue = u_StartAddress + I2C_MAX_DELAY;
      u_DelayedOverflowValue = TIMER_MAX_TIME - u_StartAddress + I2C_MAX_DELAY;

      if(u_StartAddress <= TIMER_MAX_TIME - I2C_MAX_DELAY)
      {
        // Wait for RxNE to set or max time to pass
        while(!((REG32(I2C1_SR1) & I2C1_SR1_RXNE) || (__HAL_TIM_GET_COUNTER(&htim10) >= u_DelayedValue)));
      }
      else
      {
        // Wait for RxNE to set or max time to pass
        while(!((REG32(I2C1_SR1) & I2C1_SR1_RXNE) || (__HAL_TIM_GET_COUNTER(&htim10) >= u_DelayedOverflowValue)));
      }
      // Clear the AF bit if it sets
      REG32(I2C1_SR1) &= ~I2C1_SR1_AF;

      // 2.d) Read the data from the DR
      // Copy the data into the buffer
      u_Buffer[u_Size - u_Remaining] = REG32(I2C1_DR);
      // Clear the BERR flag
      REG32(I2C1_SR1) &= ~I2C1_SR_BERR;

      // 2.e) Generate the acknowledgment by setting the ACK (bit 10 in CR1)
      // Set the ACK bit to acknowledge the data received
      REG32(I2C1_CR1) |= I2C1_CR1_ACK;
      u_Remaining--;
    }

    // Read the second last byte
    u_StartAddress = __HAL_TIM_GET_COUNTER(&htim10);
    u_DelayedValue = u_StartAddress + I2C_MAX_DELAY;
    u_DelayedOverflowValue = TIMER_MAX_TIME - u_StartAddress + I2C_MAX_DELAY;

    if(u_StartAddress <= TIMER_MAX_TIME - I2C_MAX_DELAY)
    {
      // Wait for RxNE to set or max time to pass
      while(!((REG32(I2C1_SR1) & I2C1_SR1_RXNE) || (__HAL_TIM_GET_COUNTER(&htim10) >= u_DelayedValue)));
    }
    else
    {
      // Wait for RxNE to set or max time to pass
      while(!((REG32(I2C1_SR1) & I2C1_SR1_RXNE) || (__HAL_TIM_GET_COUNTER(&htim10) >= u_DelayedOverflowValue)));
    }
    // Clear the AF bit if it sets
    REG32(I2C1_SR1) &= ~I2C1_SR1_AF;
    u_Buffer[u_Size - u_Remaining] = REG32(I2C1_DR);
    // Clear the BERR flag
    REG32(I2C1_SR1) &= ~I2C1_SR_BERR;

    // 2.f) To generate the nonacknowledge pulse after the last received data byte, the ACK bit must be cleared just after reading the second last data byte (after second last RxNE event)
    // Clear the ACK bit
    REG32(I2C1_CR1) &= ~I2C1_CR1_ACK;

    // 2.g) In order to generate the stop/restart condition, software must set the stop/start bit after reading the second last data byte (after the second last RxNE event)
    // Stop I2C
    REG32(I2C1_CR1) |= I2C1_CR1_STOP;
    u_Remaining--;

    // Read the last byte
    u_StartAddress = __HAL_TIM_GET_COUNTER(&htim10);
    u_DelayedValue = u_StartAddress + I2C_MAX_DELAY;
    u_DelayedOverflowValue = TIMER_MAX_TIME - u_StartAddress + I2C_MAX_DELAY;

    if(u_StartAddress <= TIMER_MAX_TIME - I2C_MAX_DELAY)
    {
      // Wait for RxNE to set or max time to pass
      while(!((REG32(I2C1_SR1) & I2C1_SR1_RXNE) || (__HAL_TIM_GET_COUNTER(&htim10) >= u_DelayedValue)));
    }
    else
    {
      // Wait for RxNE to set or max time to pass
      while(!((REG32(I2C1_SR1) & I2C1_SR1_RXNE) || (__HAL_TIM_GET_COUNTER(&htim10) >= u_DelayedOverflowValue)));
    }
    // Clear the AF bit if it sets
    REG32(I2C1_SR1) &= ~I2C1_SR1_AF;
    // Copy the data into the buffer
    u_Buffer[u_Size - u_Remaining] = REG32(I2C1_DR);
    // Clear the BERR flag
    REG32(I2C1_SR1) &= ~I2C1_SR_BERR;
  }
  return u_Temp;
}
