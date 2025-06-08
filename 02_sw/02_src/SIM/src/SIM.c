/// @file SIM.c
/// @brief Main file used for manipulating different operations for SIM800L module
/// @author Aleksandra Petrovic

#include"MSGM.h"
#include "SIM800L_cfg.h"

/// Buffer where complex messages including phone numbers will be written to
static uint8_t u_CoordBuf[COORDINATES_BUFFER_LENGTH] = {0};
/// Initial SIM function should be IdleFunction and it shouldn't be changed until SIM module activates for call/message operations
static e_SIM_Function e_PreviosFunction = IdleFunction;
/// Used for storing unprocessed coordinates received via UART3 from GPS module
static uint8_t *p_Coordinates = {0};
/// Used to indicate if the semaphore should be released or the SIM functions are still executing
static volatile boolean b_SemaphoreFlag = b_FALSE;

/// @brief Function used for writing commands for SIM800L module into a buffer.
///
/// @pre SIM800L must be configured first.
/// @post None
/// @param uint8_t *u_Buffer, uint8_t u_Cnt, uint8_t *u_Message
///
/// @return uint8_t u_Cnt
///
/// @globals None
///
/// @InOutCorelation Function writes message characters into a temporary buffer.
/// @callsequence
///   @startuml "v_WriteIntoBuffer.png"
///     title "Sequence diagram for function v_WriteIntoBuffer"
///     -> SIM: v_WriteIntoBuffer(uint8_t *u_Buffer, uint8_t u_Cnt, uint8_t *u_Message)
///     SIM++
///       loop Keeps going through elements until s NULL character is reached.
///         rnote over SIM: Store message characters into a temporary buffer.
///       end
///     SIM--
///     <- SIM://Returns uint8_t value of counter.//
///   @enduml

static uint8_t v_WriteIntoBuffer(uint8_t *u_Buffer, uint8_t u_Cnt, uint8_t *u_Message);

static uint8_t v_WriteIntoBuffer(uint8_t *u_Buffer, uint8_t u_Cnt, uint8_t *u_Message)
{
  // Until NULL character is reached, keep going through the message characters
  while(*u_Message != 0)
  {
	// Write message characters into the parsed buffer and increment the counter
	u_Buffer[u_Cnt] = *u_Message++;
	u_Cnt++;
  }
  return u_Cnt;
}

/// @brief Function used for ending commands for SIM800L module.
///
/// @pre SIM800L command must be sent first
/// @post None
/// @param None
///
/// @return None
///
/// @globals None
///
/// @InOutCorelation Function sends commands for SIM800L module.
/// @callsequence
///   @startuml "v_EndOfCommand.png"
///     title "Sequence diagram for function v_EndOfCommand"
///     -> SIM: v_EndOfCommand()
///     SIM++
///       UARTM -> SIM: UARTM3_v_SendChar(CARRIAGE_RETURN)
///       UARTM -> SIM: UARTM3_v_SendChar(LINE_FEED)
///     SIM--
///     <- SIM
///   @enduml

static void v_EndOfCommand(void);

static void v_EndOfCommand()
{
  // End of the command - Carriage Return in ASCII
  UARTM3_v_SendChar(CARRIAGE_RETURN);
  // End of the command - Line Feed in ASCII
  UARTM3_v_SendChar(LINE_FEED);
}

/// @brief Function used for sending commands for SIM800L module.
///
/// @pre SIM800L must be configured
/// @post None
/// @param e_Command e_SIM_Command
///
/// @return None
///
/// @globals None
///
/// @InOutCorelation Function sends commands for SIM800L module.
/// @callsequence
///   @startuml "v_SendCommand.png"
///     title "Sequence diagram for function v_SendCommand"
///     -> SIM: v_SendCommand(e_Command e_SIM_Command)
///     SIM++
///       loop Loop goes through dictionary of commands in order to find corresponding command to parsed one.
///         opt if parsed command matches an existing one
///           UARTM -> SIM: UARTM3_v_SendString(SIM800L_Dictionary[u_Cnt].Config_Command)
///         end
///       end
///       SIM -> SIM: v_EndOfCommand()
///     SIM--
///     <- SIM
///   @enduml

static void v_SendCommand(e_Command e_SIM_Command);

static void v_SendCommand(e_Command e_SIM_Command)
{
  uint8_t u_Cnt = 0;
  // Used for exiting the loop when correct command is found
  boolean b_CommandFound = b_FALSE;
  // Search the dictionary in order to find corresponding AT command
  while((u_Cnt < SIM800L_u_DictionaryLength) && (b_CommandFound == b_FALSE))
  {
	// Check if parsed command matches any command in dictionary
	if(SIM800L_t_Dictionary[u_Cnt].e_SIM800L_Command == e_SIM_Command)
	{
	  // If AT command is found, parse its text via UART
	  UARTM3_v_SendString(SIM800L_t_Dictionary[u_Cnt].Config_Command);
	  b_CommandFound = b_TRUE;
	}
	u_Cnt++;
  }
  b_CommandFound = b_FALSE;
  v_EndOfCommand();
}

void SIM_v_Setup()
{
  uint8_t u_Cnt = 0;

  //e_SIM_Response e_Response = SIM_v_ReadResponse();
  do
  {
	// Re-send AT command to make sure that it gets the OK back
    v_SendCommand(AT);
    //e_Response = SIM_v_ReadResponse();
    u_Cnt++;
  }
  // Once the handshake test is successful, OK message will be received
  while(u_Cnt < 10);//(e_Response != OK && u_Cnt < 10);

  // Signal quality test, value range is 0-31 , 31 is the best
  v_SendCommand(CSQ);

  // Read SIM information to confirm whether the SIM is plugged
  v_SendCommand(CCID);

  // Check whether it has registered in the network
  v_SendCommand(CREG);
}

void SIM_v_ReceiveMessage()
{
  // Once the handshake test is successful, OK message will be received
  v_SendCommand(AT);

  // Configuring TEXT mode
  v_SendCommand(CMGF);

  // AT Command to receive a live SMS
  v_SendCommand(CNMI);
}

uint8_t * SIM_p_ReceiveCoordinates()
{
  // Read received message
  SIM_v_ReceiveMessage();
  // Store the content of the message and return its value
  return u_CoordBuf;
}

void SIM_v_EndCall()
{
  // Used to end call
  v_SendCommand(ATH);
}

void SIM_v_CallNumber(uint8_t *u_Number)
{
  uint8_t u_Buffer[50u] = {0u};
  // Message used for configuring mode for making calls
  uint8_t *u_NumCfg = (uint8_t *)("ATD+ ");
  uint8_t *u_CallCmd = (uint8_t *)(";");
  uint8_t u_Cnt = 0;

  // Goes through elements of calling configuration command and writes them into a temporary buffer and updates u_Cnt counter
  u_Cnt = v_WriteIntoBuffer(u_Buffer, u_Cnt, u_NumCfg);
  // Adds number characters to the temporary buffer so the whole message will be sent via UART and updates u_Cnt counter
  u_Cnt = v_WriteIntoBuffer(u_Buffer, u_Cnt, u_Number);
  // ';' character is used to represent the end of call number command
  u_Cnt = v_WriteIntoBuffer(u_Buffer, u_Cnt, u_CallCmd);

  // Once the handshake test is successful, OK message will be received
  v_SendCommand(AT);

  // Call the number
  UARTM3_v_SendString(u_Buffer);
  v_EndOfCommand();
}

void SIM_v_Call(e_SIM_KnownCaller e_Caller)
{
  uint8_t u_Cnt = 0;
  boolean b_Flag = b_FALSE;
  // Go through the dictionary until valid caller is reached
  while(u_Cnt < SIM800L_u_CallerDictionaryLength && b_Flag == b_FALSE)
  {
	// Compare parsed name to the names in the dictionary
	if(e_Caller == SIM800L_t_CallerDictionary[u_Cnt].u_CallerName)
	{
	  // If correct name is found, call the number corresponding with that name
	  SIM_v_CallNumber(SIM800L_t_CallerDictionary[u_Cnt].u_Number);
	  b_Flag = b_TRUE;
	}
	u_Cnt++;
  }
}

void SIM_v_SendMessage(uint8_t *u_Message, uint8_t *u_Number)
{
  // Buffer used to store number and number configuration command
  uint8_t u_Buffer[SIM800L_MESSAGE_BUFFER_LENGTH] = {0u};
  uint8_t *u_NumCfg = (uint8_t*)"AT+CMGS=";
  uint8_t u_Cnt = 0;

  // Goes through elements of sending message configuration command and writes them into a temporary buffer and updates u_Cnt counter
  u_Cnt = v_WriteIntoBuffer(u_Buffer, u_Cnt, u_NumCfg);

  // Adds number characters to the temporary buffer so the whole message will be sent via UART and updates u_Cnt counter
  u_Cnt = v_WriteIntoBuffer(u_Buffer, u_Cnt, u_Number);

  // Once the handshake test is successful, OK message will be received
  v_SendCommand(AT);

  // Configuring TEXT mode
  v_SendCommand(CMGF);

  // Number to which the SMS should be sent
  UARTM3_v_SendString(u_Buffer);
  v_EndOfCommand();

  // Sending text of the SMS
  UARTM3_v_SendString(u_Message);
  v_EndOfCommand();

  // ESC in ASCII - indicates the end of the transaction
  UARTM3_v_SendChar(ESC);
  v_EndOfCommand();
}

void SIM_v_SendCoordinates(e_SIM_KnownCaller e_Caller)
{
  uint8_t u_Cnt = 0;
  boolean b_Flag = b_FALSE;
  // Go through the dictionary until valid caller is reached
  while(u_Cnt < SIM800L_u_CallerDictionaryLength && b_Flag == b_FALSE)
  {
    // Compare parsed name to the names in the dictionary
    if(e_Caller == SIM800L_t_CallerDictionary[u_Cnt].u_CallerName)
    {
 	  b_Flag = b_TRUE;
 	  // Send unprocessed coordinates to set number
 	  SIM_v_SendMessage(p_Coordinates, SIM800L_t_CallerDictionary[u_Cnt].u_Number);
 	}
    u_Cnt++;
  }
  u_Cnt = 0;
  // Empty SIM buffer first so the old data doesn't affect the new data
  for(u_Cnt = 0; u_Cnt < COORDINATES_BUFFER_LENGTH; u_Cnt++)
  {
	u_CoordBuf[u_Cnt] = 0u;
  }
  u_Cnt = 0;
  // Store coordinates into buffer so they can be read
  v_WriteIntoBuffer(u_CoordBuf, u_Cnt, p_Coordinates);
}

/*static e_SIM_Response v_ReadResponse(void);

static e_SIM_Response v_ReadResponse()
{
  uint8_t u_Buffer[SIM800L_RESPONSE_LENGTH] = {0u};
  uint8_t u_Cnt = 0;
  uint8_t u_Cntr = 0;
  uint8_t u_Counter = 0;
  boolean b_DictionaryFlag = b_FALSE;
  uint8_t u_TrueFlag = 0;

  // Until NULL character is reached or number of characters reaches maximum length of the response, keep reading messages from UART2
  while(UARTM2_u_GetChar() != 0 && u_Cnt < SIM800L_RESPONSE_LENGTH)
  {
    // Write into temporary buffer characters read from UART2
	u_Buffer[u_Cnt] = UARTM2_u_GetChar();
	u_Cnt++;
  }

  // Reset the counter so it is used to go through the elements of response dictionary
  u_Cnt = 0;

  // Goes through elements of SIM800L_ResponseDictionary
  while(u_Cnt < SIM800L_u_ResponseDictionaryLength && b_DictionaryFlag == b_FALSE)
  {
	// Find the response in dictionary to check if it corresponds with received response
    while(u_Counter < SIM800L_t_ResponseDictionary[u_Counter].u_ResponseLength)
    {
      // Goes through characters of a response in the dictionary
      while(u_Cntr < SIM800L_t_ResponseDictionary[u_Counter].u_ReceiveMessage[u_Counter])
      {
        if(u_Buffer[u_Counter] == SIM800L_t_ResponseDictionary[u_Counter].u_ReceiveMessage[u_Cntr])
        {
	      // Set the flag to indicate that read response corresponds with a response in dictionary
//	      SIM800L_ResponseDictionary[u_Counter].b_Flag = b_TRUE;
	      b_DictionaryFlag = b_TRUE;
	      u_TrueFlag = u_Counter;
        }
        else
        {
          // Read response doesn't correspond with any response in dictionary
//          SIM800L_ResponseDictionary[u_Counter].b_Flag = b_FALSE;
        }
        u_Cntr++;
      }
      u_Counter++;
    }
    u_Cnt++;
  }

  if(b_DictionaryFlag == b_TRUE)
  {
    // If response from SIM corresponds with a response from dictionary, return its value
	return SIM800L_t_ResponseDictionary[u_TrueFlag].e_Response;
  }
  else
  {
	// If response from SIM doesn't correspond with any of the predefined responses or there is no response at all, return no response
	return NO_RSP;
  }
}

static boolean SIM_b_KnownCallerCheck(void);

static boolean SIM_b_KnownCallerCheck()
{
  e_SIM_Response e_Rsp = v_ReadResponse();
  boolean b_Flag = b_FALSE;
  uint8_t u_Buffer[SIM800L_RESPONSE_LENGTH] = {0};
  uint8_t u_Cnt = 0;
  uint8_t u_DictionaryCounter = 0;

  // After the CLIP response, number of the caller is read from terminal
  if(e_Rsp == CLIP)
  {
	// Read the rest of the response to reach the number of the caller
	while(UARTM2_u_GetChar() != 0 && u_Cnt < SIM800L_RESPONSE_LENGTH)
	{
	  // + character indicates beginning of the number
	  if(UARTM2_u_GetChar() == '+')
	  {
		// " character indicates the end of the number portion of the response
		while(UARTM2_u_GetChar() != '"' && u_Cnt < SIM800L_NUMBER_LENGTH)
		{
	      // Write into temporary buffer characters read from UART2
	      u_Buffer[u_Cnt] = UARTM2_u_GetChar();
	      u_Cnt++;
	    }
	  }
	}
	// Compare the content of the buffer to the numbers in the dictionary
	while(u_DictionaryCounter < SIM800L_u_CallerDictionaryLength)
	{
      uint8_t u_Counter = 0;
      // Until the end of the written data or set flag is reached, keep going through the array
	  while(u_Counter < u_Cnt && b_Flag == b_FALSE)
	  {
		// Compare elements of the buffer to number characters in the dictionary
	    if(u_Buffer[u_Counter] == SIM800L_t_CallerDictionary -> u_Number[u_Counter])
	    {
	      SIM800L_t_CallerDictionary[u_DictionaryCounter].b_KnownCaller = b_TRUE;
		  b_Flag = b_TRUE;
	    }
	    else
	    {
	      SIM800L_t_CallerDictionary[u_DictionaryCounter].b_KnownCaller = b_FALSE;
	    }
	    u_Counter++;
	  }
	  u_DictionaryCounter++;
	}
  }
  return b_Flag;
}*/

void SIM_v_StateMachine()
{
  t_SIM_Function * t_func = SIM_p_Function();
  e_SIM_Function e_NextFunction = t_func -> e_CurrentFunction;

  // Check if current function differers from previous one to determine if the change has occurred
  if(e_PreviosFunction != e_NextFunction)
  {
    switch(e_NextFunction)
    {
      // Used when other functions finish executing
      case IdleFunction:
	      break;
	  // Used when call should be made
      case MakeCall:
    	  b_SemaphoreFlag = b_TRUE;
    	  // Stores coordinates so they won't be rewritten
    	  p_Coordinates = MSGM_p_GetRawMessage();
    	  // Make a call to a SIM card inserted into SIM module
  	      SIM_v_Call(SIM_module);
  	      SIM800L_t_Functions -> e_CurrentFunction = EndCall;
	      break;
	  // Used when call should be ended
      case EndCall:
/*    	  e_SIM_Response e_Response = v_ReadResponse();
    	  // Wait for the ring and then reject the call
    	  if(e_Response == RING)
          {
            // Check if the known caller made the call
    	    boolean b_Check = SIM_b_KnownCallerCheck();
    	    if(b_Check == b_TRUE)
    	    {
*/
    	  // Decline the call and set flag to SendMessage in order to send raw message via SMS
    	  SIM_v_EndCall();
    	  SIM800L_t_Functions -> e_CurrentFunction = SendMessage;
/*    	    }
    	    else
    	    {
    	      UARTM2_v_SendString((uint8_t*)"Unknown caller");
    	    }
          }*/
  	      break;
  	  // Used when message should be sent
      case SendMessage:
    	  b_SemaphoreFlag = b_TRUE;
/*    	  // Loop goes through dictionary searching for the right caller in order to send him the coordinates
    	  for(uint8_t u_Cnt = 0; u_Cnt < SIM800L_u_CallerDictionaryLength; u_Cnt++)
    	  {
    		if(SIM800L_t_CallerDictionary[u_Cnt].b_KnownCaller == b_TRUE)
    		{
    		  // Send coordinates to caller with set flag
    		  SIM_v_SendCoordinates(SIM800L_t_CallerDictionary[u_Cnt].u_CallerName);
    		  // Clear the flag
    		  SIM800L_t_CallerDictionary[u_Cnt].b_KnownCaller = b_FALSE;
    		}
    	  }
*/
	      SIM_v_SendCoordinates(Aleksandra);
	      // After the message has been sent, SIM is ready for the message to be read
	      SIM800L_t_Functions -> e_CurrentFunction = ReadMessage;
	      break;
	  // Used when message should be read
      case ReadMessage:
    	  b_SemaphoreFlag = b_FALSE;
  	      break;
      default:
	      break;
    }
  }
  e_PreviosFunction = e_NextFunction;
}

t_SIM_Function * SIM_p_Function()
{
  return SIM800L_t_Functions;
}

boolean SIM_b_GetFlag()
{
  return b_SemaphoreFlag;
}
