/// @file MCP23017.c
/// @brief Main file used for manipulating different operations with MCP23017 GPIO expander
/// @author Aleksandra Petrovic

#include "cmsis_os.h"
#include "MCP23017_cfg.h"
#include "SIM.h"
#include "tim.h"

/// Counter of button presses
static volatile uint32_t u_ButtonPressed_count   = 0;
/// Counter of button releases
static volatile uint32_t u_ButtonReleased_count  = 0;
/// Flag that indicates if button is pressed
static volatile boolean b_PressedButton		  	 = b_FALSE;

void MCP23017_v_EXTI1_Configuration()
{
  // Enable the SYSCFG (System Configuration controller) peripheral
  REG32(RCC_APB2ENR) |= RCC_APB2ENR_SYSCFGEN_ENABLE;
  // EXTI1 line
  SYSCFG->EXTICR[0] |= EXTI1_EN;
  // Enable GPIOB CLOCK
  REG32(RCC_AHB1ENR) |= RCC_AHB1ENR_GPIOB_CLOCK;
  // Input (reset state) for PB1
  REG32(GPIOB_MODER) &= ~GPIOB_PB1_INPUT;
  // Pull-up for PB1
  REG32(GPIOB_PUPDR) |= GPIOB_PB1_PULLUP;
  // Interrupt mask on line 1, it is not masked
  REG32(EXTI_IMR) |= EXTI_IMR_PB1;
  // Rising trigger event configuration bit of line 1 - disabled
  REG32(EXTI_RTSR) &= ~EXTI_RTSR_PB1;
  // Falling trigger event configuration bit of line 1 - enabled
  REG32(EXTI_FTSR) |= EXTI_FTSR_PB1;
  // Setting NVIC interrupt priority
  NVIC_SetPriority(EXTI1_IRQn, 14);
  // Enable interrupt service routine
  NVIC_EnableIRQ(EXTI1_IRQn);
}

void EXTI1_IRQHandler(void)
{
  if(REG32(EXTI_PR) && EXTI_PR_PB1)
  {
	// Set SIM function as MakeCall when button has been pressed
    t_SIM_Function * t_func = SIM_p_Function();
  	t_func -> e_CurrentFunction = MakeCall;
  	// Reset the button flag
    b_PressedButton = b_FALSE;
    // Clear the interrupt
    REG32(EXTI_PR) &= EXTI_PR_PB1;
  }
}

/// @brief Function used for writing data
///
/// @pre MCP23017 must be configured
/// @post None
/// @param uint8_t u_Address, uint8_t u_Reg, uint8_t u_Data
///
/// @return None
///
/// @globals None
///
/// @InOutCorelation MCP23017 function for writing data.
/// @callsequence
///   @startuml "v_Write.png"
///     title "Sequence diagram for function v_Write"
///     -> MCP23017: v_Write(uint8_t u_Address, uint8_t u_Reg, uint8_t u_Data)
///     MCP23017++
///       I2C -> MCP23017: I2C_v_Start()
///       I2C -> MCP23017: I2C_u_SendAddress(u_Address)
///       I2C -> MCP23017: I2C_v_Write(u_Reg)
///       I2C -> MCP23017: I2C_v_Write(u_Data)
///       I2C -> MCP23017: I2C_v_Stop()
///       rnote over MCP23017: Parsed data is written into the register.
///     <- MCP23017
///     MCP23017--
///   @enduml

static void v_Write(uint8_t u_Address, uint8_t u_Reg, uint8_t u_Data);

static void v_Write(uint8_t u_Address, uint8_t u_Reg, uint8_t u_Data)
{
  // Start the I2C
  I2C_v_Start();
  // Send the address of the device
  I2C_u_SendAddress(u_Address);
  // Send the address of the register where data will be written to
  I2C_v_Write(u_Reg);
  // Send the data
  I2C_v_Write(u_Data);
  // Stop the I2C
  I2C_v_Stop();
}

/// @brief Function used for reading the data
///
/// @pre MCP23017 must be configured
/// @post None
/// @param uint8_t u_Address, uint8_t u_Reg, uint8_t *u_Buffer, uint8_t u_Size
///
/// @return None
///
/// @globals None
///
/// @InOutCorelation MCP23017 function for reading data.
/// @callsequence
///   @startuml "v_Read.png"
///     title "Sequence diagram for function v_Read"
///     -> MCP23017: v_Read(uint8_t u_Address, uint8_t u_Reg, uint8_t *u_Buffer, uint8_t u_Size)
///     MCP23017++
///       I2C -> MCP23017: I2C_v_Start()
///       I2C -> MCP23017: I2C_u_SendAddress(u_Address)
///       I2C -> MCP23017: I2C_v_Write(u_Reg)
///       I2C -> MCP23017: I2C_v_Start()
///       I2C -> MCP23017: I2C_u_Read(u_Address + 0x01, u_Buffer, u_Size)
///       I2C -> MCP23017: I2C_v_Stop()
///       rnote over MCP23017: Read data is stored into u_Bufer.
///     <- MCP23017
///     MCP23017--
///   @enduml

static void v_Read(uint8_t u_Address, uint8_t u_Reg, uint8_t *u_Buffer, uint8_t u_Size);

static void v_Read(uint8_t u_Address, uint8_t u_Reg, uint8_t *u_Buffer, uint8_t u_Size)
{
  // Start the I2C
  I2C_v_Start();
  // Send the address of the device
  I2C_u_SendAddress(u_Address);
  // Send the address of the register where data will be read from
  I2C_v_Write(u_Reg);
  // Send the restart condition
  I2C_v_Start();
  // Read the data
  I2C_u_Read(u_Address + 0x01, u_Buffer, u_Size);
  // Stop the I2C
  I2C_v_Stop();
}

void MCP23017_v_Init()
{
  // Set GPIOA as input
  v_Write(MCP23017_ADDRESS << 1, MCP23017_IODIRA, MCP23017_GPIOA_INPUT);
  // Enable pull-up on GPIOA
  v_Write(MCP23017_ADDRESS << 1, MCP23017_GPPUA, MCP23017_GPIOA_PULLUP);
  // Set GPIOB as output
  v_Write(MCP23017_ADDRESS << 1, MCP23017_IODIRB, MCP23017_GPIOB_OUTPUT);

  // Interrupt configuration
  v_Write(MCP23017_ADDRESS << 1, MCP23017_IOCONA, MCP23017_GPIOA_IOCON);
  // Write 1s into GPINTENA register to enable GPIO input pin for interrupt-on-change event
  v_Write(MCP23017_ADDRESS << 1, MCP23017_GPINTENA, MCP23017_GPIOA_INTERRUP_ENABLE);
  // Write 1s indicate that associated pin value is compared for interrupt-on-change
  v_Write(MCP23017_ADDRESS << 1, MCP23017_INTCONA, MCP23017_GPIOA_INTERRUP_ENABLE);
  // If value on pin differs from MCP23017_GPIOA_DEFVAL, an interrupt occurred
  v_Write(MCP23017_ADDRESS << 1, MCP23017_DEFVALA, MCP23017_GPIOA_DEFVAL);
}

/// @brief Function used for reading button and turning LED on
///
/// @pre Button and LEDs must be configure
/// @post None
/// @param uint8_t u_Value
///
/// @return None
///
/// @globals None
///
/// @InOutCorelation Function reads the button and turns the correct LED on.
/// @callsequence
///   @startuml "v_TurnLED.png"
///     title "Sequence diagram for function v_TurnLED"
///     -> MCP23017: v_TurnLED(uint8_t u_Value)
///     MCP23017++
///       MCP23017 -> MCP23017:v_Write(MCP23017_ADDRESS << 1, MCP23017_GPIOB, u_Value)
///       rnote over MCP23017: Turns LED which values is passed.
///     <- MCP23017
///     MCP23017--
///   @enduml

static void v_TurnLED(uint8_t u_Value);

static void v_TurnLED(uint8_t u_Value)
{
  // Write 1s for bits where LEDs should be turned on
  v_Write(MCP23017_ADDRESS << 1, MCP23017_GPIOB, u_Value);
}

/// @brief Function used for reading button
///
/// @pre Button must be configured
/// @post None
/// @param None
///
/// @return uint8_t u_Value
///
/// @globals None
///
/// @InOutCorelation Function reads the button and returns its value.
/// @callsequence
///   @startuml "u_Button_Read.png"
///     title "Sequence diagram for function u_Button_Read"
///     -> MCP23017: u_Button_Read()
///     MCP23017++
///       MCP23017 -> MCP23017: v_Read(MCP23017_ADDRESS << 1, MCP23017_GPIOA, &u_Value, 1);
///       rnote over MCP23017: Read values are written into u_Value.
///     <- MCP23017:// Returns a uint8_t value that is read.//
///     MCP23017--
///   @enduml

static uint8_t u_Button_Read(void);

static uint8_t u_Button_Read()
{
  // u_Value is used as buffer where read data will be written into
  uint8_t u_Value = 0;
  v_Read(MCP23017_ADDRESS << 1, MCP23017_GPIOA, &u_Value, 1);
  return u_Value;
}

void MCP23017_v_TurnLEDviaCoordinates()
{
  // Button is active low so here is only GPA7 active
  uint8_t u_ButtonState = u_Button_Read();
  t_SIM_Function * t_func = SIM_p_Function();
  // When button has been pressed calculate bearing and turn correct LED on
  if(u_ButtonState == BUTTON_PRESSED)
  {
    b_PressedButton = b_TRUE;
  }
  else
  {
    b_PressedButton = b_FALSE;
  }
  if(t_func -> e_CurrentFunction == ReadMessage)
  {
    // Used to calculate direction of the car
    uint16_t u_Bearing = CALCM_u_CalculateBearing();
    uint8_t u_LEDs = 0x00;

    // Loop goes through MCP23017_t_Ranges to determine which LED should be turned on
    for(uint16_t u_Count = 0; u_Count < MCP23017_u_Length; u_Count++)
    {
      // Check if bearing is inside the range
      if(u_Bearing >= MCP23017_t_Ranges[u_Count].u_Low && u_Bearing <= MCP23017_t_Ranges[u_Count].u_High)
      {
        // Used to set the flag of the LED which should be turned on
        MCP23017_t_Ranges[u_Count].b_TurnedON = b_TRUE;
        // Store value of LED that should be turned on
        uint8_t u_LEDnum = MCP23017_t_Ranges[u_Count].u_LED;
        // Add value of LEDs with set flag to turn on all of them
        u_LEDs += u_LEDnum;
        v_TurnLED(u_LEDs);
      }
      else
      {
        MCP23017_t_Ranges[u_Count].b_TurnedON = b_FALSE;
      }
    }
    t_func -> e_CurrentFunction = IdleFunction;
  }
}
