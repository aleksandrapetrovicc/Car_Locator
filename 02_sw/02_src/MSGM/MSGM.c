/// @file MSGM.c
/// @brief Main file used to manipulate different types of messages
/// @author Aleksandra Petrovic

#include "MSGM.h"
#include "UARTM.h"
#include "UARTM_cfg.h"
#include "CALCM.h"
#include "SIM.h"
#include "FreeRTOS.h"
#include "projdefs.h"
#include <stdio.h>

uint8_t                u_data                                           = 0u;          // Initiate data in state machine to 0
uint8_t                a_TempBuffer[50u]                                = {0u};        // Set all the elements of a temporary buffer, used to store coordinates, to 0
t_MessageElement       MSGM_t_MessageBuffer[MSGM_MESSAGE_BUFFER_LENGTH] = {0u};        // Set message buffer elements to 0 used to sort messages
t_CoordinatesStructure MSGM_t_Coordinates                               = {0u};        // Structure that saves final, parsed coordinates is initiated to 0
uint8_t 			   u_RawMessageBuffer[50u]							= {0u};        // Set raw message buffer to 0

t_CircularBuffer t_RingBuffer =
{
  {0u},                                                                                // Set all of the elements in the ring buffer to 0
  0u,                                                                                  // Set write pointer to 0
  0u,                                                                                  // Set read pointer to 0
  0u                                                                                   // Set counter of processed elements to 0
};

t_CircularBuffer t_SIM_RingBuffer =
{
		{0u},
		0u,
		0u,
		0u
};

t_CircularBuffer *MSGM_a_RingBuffers[NUM_OF_RING_BUFFERS] =
{
  &t_RingBuffer
};

t_MessageElement MSGM_t_Dictionary[MSGM_DICTIONARY_LENGTH] = {
    {
      (uint8_t*) "LED_ON____",
      LEDM, LED_ON____, 6u
    }
    ,
    {
      (uint8_t*) "LED_OFF___",
      LEDM, LED_OFF___, 7u
    }
};

void MSGM_v_Sort(uint8_t *UARTM_a_array)
{
  boolean        b_WordFound = b_FALSE;
  uint8_t        u_Index     = 0u;
  uint8_t        u_Length    = 0u;
  uint8_t        u_Cnt       = 0u;
  static uint8_t Buffer_slot = 0u;

  while (UARTM_a_array[u_Index] != '*')
  {
    u_Index++;
  }

  u_Length = u_Index;

  for (u_Cnt = 0u; u_Cnt < MSGM_DICTIONARY_LENGTH; u_Cnt++)
  {
    if (MSGM_t_Dictionary[u_Cnt].u_Length == u_Length)
    {
      for (u_Index = 0u; u_Index < u_Length; u_Index++)
      {
        if (UARTM_a_array[u_Index] != MSGM_t_Dictionary[u_Cnt].u_RawMessage[u_Index])
        {
          b_WordFound = b_FALSE;
          break;
        }
        else
        {
          b_WordFound = b_TRUE;
        }
      }
    }
    if (b_WordFound == b_TRUE)
    {
      MSGM_t_MessageBuffer[Buffer_slot].e_ID = MSGM_t_Dictionary[u_Cnt].e_ID;
      MSGM_t_MessageBuffer[Buffer_slot].e_Msg = MSGM_t_Dictionary[u_Cnt].e_Msg;

      if (Buffer_slot == MSGM_MESSAGE_BUFFER_SLOT)
      {
        Buffer_slot++;
        Buffer_slot = 0u;
      }
      else
      {
        Buffer_slot++;
      }
      b_WordFound = b_FALSE;
    }
  }
  UARTM_v_ClearBuffer();
}

void MSGM_v_ClearMessage(uint8_t u_Index)
{
  MSGM_t_MessageBuffer[u_Index].u_RawMessage = (uint8_t*) "";
  MSGM_t_MessageBuffer[u_Index].e_Msg = DEFAULT_MSG;
  MSGM_t_MessageBuffer[u_Index].e_ID = DEFAULT_ID;
  MSGM_t_MessageBuffer[u_Index].u_Length = 0u;
}

e_Message MSGM_e_MessageRetrieve(e_ReceiverID e_ID)
{
  uint8_t u_Index = 0u;
  e_Message e_MessageReturn;

  for (u_Index = 0; u_Index < MSGM_MESSAGE_BUFFER_LENGTH; u_Index++)
  {
    if (MSGM_t_MessageBuffer[u_Index].e_ID == e_ID)
    {
      e_MessageReturn = MSGM_t_MessageBuffer[u_Index].e_Msg;
      MSGM_v_ClearMessage(u_Index);
      break;
    }
  }
  return e_MessageReturn;
}

boolean MSGM_b_CircularBufferIsEmpty(e_RingBuffers e_BufferID)
{
  return (t_RingBuffer.u_head == t_RingBuffer.u_tail);          // Check if the ring buffer is empty of data
}

uint8_t MSGM_u_CircularBufferPush(e_RingBuffers e_BufferID, uint8_t UARTM_u_data)
{

  if (e_BufferID <= NUM_OF_RING_BUFFERS)
  {
    t_RingBuffer = *MSGM_a_RingBuffers[e_BufferID];
  }

  if (t_RingBuffer.u_count == MSGM_RING_BUFFER_LENGTH)                                              // Checks if the ring buffer is full
  {
    return 0;
  }

  if ((t_RingBuffer.u_head + 1u) == (t_RingBuffer.u_tail))                                          // Checks for unintended behavior if read pointer is ahead of write pointer
  {
    return 0xFF;
  }

  t_RingBuffer.u_buffer[t_RingBuffer.u_head] = UARTM_u_data;                                        // Push the data from interrupt to ring buffer
  t_RingBuffer.u_head                        = (t_RingBuffer.u_head + 1) % MSGM_RING_BUFFER_LENGTH; // Check if the end of a ring buffer is being reached
  t_RingBuffer.u_count++;                                                                           // Push the count ahead to signalize the data is being transfered to the ring buffer

  return 1;
}

uint8_t MSGM_u_CircularBufferPop(e_RingBuffers e_BufferID)
{

  uint8_t u_pop_data = 0u;

  if (e_BufferID <= NUM_OF_RING_BUFFERS)
  {
    t_RingBuffer = *MSGM_a_RingBuffers[e_BufferID];
  }

  if (t_RingBuffer.u_count == 0u)                                              // if the u_head == u_tail, we don't have any data
  {
    return 0xFF;
  }

  u_pop_data = t_RingBuffer.u_buffer[t_RingBuffer.u_tail];                    // Fetch the data from ring buffer and save it into a temporary variable
  t_RingBuffer.u_tail = (t_RingBuffer.u_tail + 1) % MSGM_RING_BUFFER_LENGTH;  // Checks if the end of a ring buffer is reached
  t_RingBuffer.u_count--;                                                     // Lower the count variable that signalizes data is successfully popped
  t_RingBuffer.u_buffer[t_RingBuffer.u_tail - 1u] = 0u;                       // Removes the popped data out of ring buffer
  return u_pop_data;                                                          // Returns the value of the data being read
}

void MSGM_t_GetCoordinates(uint8_t *TempBuffer)
{
  uint8_t u_index = 0u;
  uint8_t u_start = 1u;

  while (TempBuffer[u_start] != ',')                                   // Searches for ',' because of a message format
  {
    MSGM_t_Coordinates.a_Latitude[u_index++] = TempBuffer[u_start++];  // Transfer the latitude portion of a message from temporary buffer to coordinates structure
  }
  u_start++;
  MSGM_t_Coordinates.u_LatDirection = TempBuffer[u_start];             // Transfer the latitude direction portion of a message from temporary buffer to coordinates structure

  u_start += 2u;
  u_index = 0u;
  while (TempBuffer[u_start] != ',')                                   // Searches for ',' because of a message format
  {
    MSGM_t_Coordinates.a_Longitude[u_index++] = TempBuffer[u_start++]; // Transfer the longitude portion of a message from temporary buffer to coordinates structure
  }
  u_start++;

  MSGM_t_Coordinates.u_LonDirection = TempBuffer[u_start];             // Transfer the longitude direction portion of a message from temporary buffer to coordinates structure
}

void MSGM_v_StateMachine()
{
  static e_SystemState  e_NextState = Idle_State;
  static uint8_t u_Index     = 0u;
  uint8_t u_RawStart = 1u;
  uint8_t u_RawEnd = 0u;

  for (uint8_t u_idx = 0u; u_idx < COORDINATES_BUFFER_LENGTH; u_idx++)
  {
    switch (e_NextState)
    {
    case Idle_State:
      if (MSGM_b_CircularBufferIsEmpty(RING_BUFFER1))
      {
      }
      else
      {
        u_data = MSGM_u_CircularBufferPop(RING_BUFFER1);  // Fetch the data from circular buffer
        if (u_data == '$')                                // Checks for '$' sign that signalizes beginning of the message
        {
          e_NextState = GPS_State;                        // Go to GPS state
        }
      }
      break;

    case GPS_State:
      u_data = MSGM_u_CircularBufferPop(RING_BUFFER1);    // Fetch the data from circular buffer
      // There are two valid formats, GPGLL and GPRMC
      if (u_data == 'L')//if (u_data == 'M')//if (u_data == 'L')
      {
        u_data = MSGM_u_CircularBufferPop(RING_BUFFER1);
        // Letter A indicates valid coordinates
        if (u_data == 'L')//if (u_data == 'A')  //if (u_data == 'L')                                // If this statement is true the message about to be received contains coordinates
        {
          e_NextState = Message_State;                    // Go to Message state
        }
        else
        {
          e_NextState = Idle_State;                       // Go to Idle state
        }
      }
      break;

    case Message_State:
      u_data = MSGM_u_CircularBufferPop(RING_BUFFER1);    // Fetch the data from circular buffer
      if (u_data == '*')                                  // Sign '*' signalizes the end of a message
      {
        uint8_t u_Cnt = 0;
        // Save the raw message in order to send it via SIM800L module
        while(u_RawStart != u_RawEnd)
        {
          u_RawMessageBuffer[u_Cnt] = a_TempBuffer[u_RawStart];
          u_RawStart++;
          u_Cnt++;
        }
        e_NextState = Transfer_State;                     // Go to Transfer state
      }
      else
      {
        if (u_Index < COORDINATES_BUFFER_LENGTH)          // Checks if the message is valid and temporary buffer isn't overflowed
        {
          if (u_data != 255)                              // Checks for empty character if every character from the ring buffer is already popped
          {
            a_TempBuffer[u_Index++] = u_data;             // Fills the buffer with relevant data
            u_RawEnd++;
          }
        }
        else
        {
          e_NextState = Idle_State;                       // Go to Idle state
          u_Index = 0u;
        }
      }
      break;

    case Transfer_State:
      u_Index = 0u;
      MSGM_t_GetCoordinates(a_TempBuffer);                // Calls for function that parses the string from temporary buffer and formats it into coordinates
      e_NextState = Idle_State;                           // Go back to Idle state
      break;
    }
  }
}

t_CoordinatesStructure * MSGM_p_GetCoordinates()
{
  return &MSGM_t_Coordinates;
}

uint8_t * MSGM_p_GetRawMessage()
{
  return u_RawMessageBuffer;
}
