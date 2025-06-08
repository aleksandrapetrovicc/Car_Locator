/// @file I2C.h
/// @brief Header file used for I2C communication
/// @author Aleksandra Petrovic

#ifndef I2C_H_
#define I2C_H_

#include "stm32f439xx.h"
#include "I2C_cfg.h"

/// Maximum value timer can count
#define TIMER_MAX_TIME 65536u
/// Value of maximum delay
#define I2C_MAX_DELAY  300u

/// @brief Function used for configuring I2C protocol
///
/// @pre None
/// @post I2C1 is set for I2C communication
/// @param None
///
/// @return None
///
/// @globals None
///
/// @InOutCorelation Function sets up registers for I2C communication.
/// @callsequence
///   @startuml "I2C_v_Configure.png"
///     title "Sequence diagram for function I2C_v_Configure"
///     -> I2C: I2C_v_Configure()
///     I2C++
///       rnote over I2C: Registers configured for I2C communication.
///     <- I2C
///     I2C--
///   @enduml

void I2C_v_Configure();

/// @brief Function used for starting I2C protocol
///
/// @pre I2C is configured
/// @post I2C1 is ready for read and write operations
/// @param None
///
/// @return None
///
/// @globals None
///
/// @InOutCorelation Function sets SB bit so START condition is generated.
/// @callsequence
///   @startuml "I2C_v_Start.png"
///     title "Sequence diagram for function I2C_v_Start"
///     -> I2C: I2C_v_Start()
///     I2C++
///       rnote over I2C: Generate start condition.
///       opt if start time of the function is smaller than max counter time - value for delay
///         loop Stay in loop until START bit sets or enough time passes.
///         end
///       else else
///         loop Stay in loop until START bit sets or enough time passes.
///         end
///       end
///     <- I2C
///     I2C--
///   @enduml

void I2C_v_Start();

/// @brief Function used for stopping I2C protocol
///
/// @pre I2C is started
/// @post I2C1 is disabled for read and write operations
/// @param None
///
/// @return None
///
/// @globals None
///
/// @InOutCorelation Function sets STOP bit to disable I2C protocol.
/// @callsequence
///   @startuml "I2C_v_Stop.png"
///     title "Sequence diagram for function I2C_v_Stop"
///     -> I2C: I2C_v_Stop()
///     I2C++
///       rnote over I2C: Sets STOP bit.
///     <- I2C
///     I2C--
///   @enduml

void I2C_v_Stop();

/// @brief Function used for writing data into DR
///
/// @pre I2C is configured
/// @post None
/// @param uint8_t u_Data
///
/// @return None
///
/// @globals None
///
/// @InOutCorelation Function writes data into DR.
/// @callsequence
///   @startuml "I2C_v_Write.png"
///     title "Sequence diagram for function I2C_v_Write"
///     -> I2C: I2C_v_Write()
///     I2C++
///       opt if start time of the function is smaller than max counter time - value for delay
///         loop Stay in loop until TXE bit sets or enough time passes.
///         end
///       else else
///         loop Stay in loop until TXE bit sets or enough time passes.
///         end
///       end
///       rnote over I2C: Clear AF and BERR bits and write data into DR.
///       opt if start time of the function is smaller than max counter time - value for delay
///         loop Stay in loop until BTF bit sets or enough time passes.
///         end
///       else else
///         loop Stay in loop until BTF bit sets or enough time passes.
///         end
///       end
///       rnote over I2C: Clear AF bit.
///     <- I2C
///     I2C--
///   @enduml

void I2C_v_Write(uint8_t u_Data);

/// @brief Function used for sending the address of slave devices or registers used for read and write operations
///
/// @pre I2C is configured
/// @post I2C1 is ready for read and write operations
/// @param uint8_t u_Address
///
/// @return uint8_t u_Temp
///
/// @globals None
///
/// @InOutCorelation Function sends address where data should be written to or read from.
/// @callsequence
///   @startuml "I2C_v_SendAddress.png"
///     title "Sequence diagram for function I2C_v_SendAddress"
///     -> I2C: I2C_v_SendAddress()
///     I2C++
///       rnote over I2C: Address is written into DR register.
///       opt if start time of the function is smaller than max counter time - value for delay
///         loop Stay in loop until ADDR bit sets or enough time passes.
///         end
///       else else
///         loop Stay in loop until ADDR bit sets or enough time passes.
///         end
///       end
///       rnote over I2C: Clear AF and ADDR bits and enable ACK.
///     <- I2C:// Returns a uint8_t value that is read for clearing the ADDR bit.//
///     I2C--
///   @enduml

uint8_t I2C_u_SendAddress(uint8_t u_Address);

/// @brief Function used for reading data from DR
///
/// @pre I2C is configured
/// @post None
/// @param uint8_t u_Address, uint8_t *u_Buffer, uint8_t u_Size
///
/// @return u_Temp
///
/// @globals None
///
/// @InOutCorelation Function reads data from DR.
/// @callsequence
///   @startuml "I2C_v_Read.png"
///     title "Sequence diagram for function I2C_v_Read"
///     -> I2C: I2C_v_Read()
///     I2C++
///       rnote over I2C: u_Remainig gets value from u_Size
///       opt if u_Size is 1
///         rnote over I2C: Address written into DR register and BERR flag cleared.
///         opt if start time of the function is smaller than max counter time - value for delay
///           loop Stay in loop until ADDR bit sets or enough time passes.
///           end
///         else else
///           loop Stay in loop until ADDR bit sets or enough time passes.
///           end
///         end
///         rnote over I2C: Clear AF, ACK and ADDR bits and stop I2C.
///         opt if start time of the function is smaller than max counter time - value for delay
///           loop Stay in loop until RxNE bit sets or enough time passes.
///           end
///         else else
///           loop Stay in loop until RxNE bit sets or enough time passes.
///           end
///         end
///         rnote over I2C: Clear AF bit, read data from DR and clear BERR flag.
///       else else
///         rnote over I2C: Address written into DR register and BERR flag cleared.
///         opt if start time of the function is smaller than max counter time - value for delay
///           loop Stay in loop until ADDR bit sets or enough time passes.
///           end
///         else else
///           loop Stay in loop until ADDR bit sets or enough time passes.
///           end
///         end
///         rnote over I2C: Clear AF and ADDR bits and stop I2C.
///         loop Stay in loop while u_Remaining is greater than 2
///           opt if start time of the function is smaller than max counter time - value for delay
///             loop Stay in loop until RxNE bit sets or enough time passes.
///             end
///           else else
///             loop Stay in loop until RxNE bit sets or enough time passes.
///             end
///           end
///           rnote over I2C: Copy the data into the buffer, clear BERR and ACK flags and decrement u_Remaining.
///         end
///         rnote over I2C: Read the second last byte
///         opt if start time of the function is smaller than max counter time - value for delay
///           loop Stay in loop until RxNE bit sets or enough time passes.
///           end
///         else else
///           loop Stay in loop until RxNE bit sets or enough time passes.
///           end
///         end
///         rnote over I2C: Clear AF, ACK and ADDR bits and stop I2C.
///         rnote over I2C: Decrement u_Remaining to reach the last byte.
///         opt if start time of the function is smaller than max counter time - value for delay
///           loop Stay in loop until RxNE bit sets or enough time passes.
///           end
///         else else
///           loop Stay in loop until RxNE bit sets or enough time passes.
///           end
///         end
///         rnote over I2C: Clear AF, copy data into the buffer and clear BERR flag.
///       end
///     <- I2C:// Returns a uint8_t value that is read for clearing the ADDR bit.//
///     I2C--
///   @enduml

uint8_t I2C_u_Read(uint8_t u_Address, uint8_t *u_Buffer, uint8_t u_Size);

#endif
