/// @file SIM.h
/// @brief Header file used for manipulating different operations for SIM800L module
/// @author Aleksandra Petrovic

#ifndef SIM_H_
#define SIM_H_

/// This enum is used for different functions of SIM module
typedef enum {
	IdleFunction,	///< Idle function for SIM800L
	MakeCall,		///< Function for making calls for SIM800L
	EndCall,		///< Function for ending a call for SIM800L
	SendMessage,	///< Function for sending a message for SIM800L
	ReadMessage		///< Function for reading a message for SIM800L
} e_SIM_Function;

/// This structure is used for manipulating SIM states and commands
typedef struct {
	e_SIM_Function e_CurrentFunction;	///< Current function of SIM800L module
	e_SIM_Function e_PreviousFunction;	///< Previous function of SIM800L module
} t_SIM_Function;

/// This enum is used for different types of messages sent to modules from UART
typedef enum {
  AT,		///< AT command is used to check if the SIM800L module is ready for usage
  CSQ,	 	///< CSQ command is used to check the signal quality, value range is 0-31 , 31 is the best
  CCID,		///< CCID command is used to confirm whether the SIM is plugged
  CREG,		///< CREG command is used to check whether it has registered in the network
  CMGF,		///< CMGF command is used t configure the text mode
  CNMI,		///< CNMI command is used to receive the live SMS
  ATH,		///< ATH command is used to decline a call
  ATA, 		///< ATH command is used to accept a call
  ATD,		///< ATD command is used to make a call
  CMGS		///< CMGS command is used to send the message
} e_Command;

/// This enum is used for different responses of SIM module
typedef enum {
	OK,			///< OK response from SIM module
	RING,		///< Response when SIM is receiving a call
	NO_CARRIER,	///< Response from SIM when it cannot make a call or send SMS
	CLIP,		///< Response after the RING response that indicates which number is calling the SIM module
	NO_RSP		///< Used when there is no response from SIM module
} e_SIM_Response;

/// This struct is used for different responses of SIM module to AT commands and calls and messages
typedef struct {
	uint8_t* u_ReceiveMessage;		///< Field used for message of the response
	uint8_t u_ResponseLength;		///< Field used for response length of SIM module
	e_ReceiverID e_ID;       	 	///< Field used to store a message unique identification number
	e_SIM_Response e_Response;		///< Field used for response name of SIM module
} t_SIM_Response;

/// This structure is used for SIM800L commands
typedef struct {
	uint8_t* Config_Command;    	///< AT command used to determine if SIM800L module is correctly configured
	e_ReceiverID e_ID;       	 	///< Field used to store a message unique identification number
	e_Command e_SIM800L_Command;	///< Message of a buffer that signalizes proper AT command
} t_SIM_Command;

/// This enum is used for listing known callers
typedef enum {
	Aleksandra, 				///< Name of the first caller in dictionary
	SIM_module					///< Name of the second caller in dictionary
} e_SIM_KnownCaller;

/// This struct is used for checking if the caller is known
typedef struct {
	uint8_t* u_Number;					///< Field used for number of the caller
	e_SIM_KnownCaller u_CallerName;		///< Field used for caller's name
	boolean b_KnownCaller;				///< Field used for indicating if the caller is known
} t_SIM_KnownCaller;

/// @brief Function used for setting up SIM800L module
///
/// @pre UART must be configured
/// @post SIM800L module is ready for sending and receiving texts and calls
/// @param None
///
/// @return None
///
/// @globals None
///
/// @InOutCorelation Used for setting up SIM800L module.
/// @callsequence
///   @startuml "SIM_v_Setup.png"
///     title "Sequence diagram for function SIM_v_Setup"
///     -> SIM: SIM_v_Setup()
///     SIM++
///       loop Re-send AT command to make sure to get the OK back.
///         SIM -> SIM: v_SendCommand(AT)
///       end
///       SIM -> SIM: v_SendCommand(CSQ)
///       SIM -> SIM: v_SendCommand(CCID)
///       SIM -> SIM: v_SendCommand(CREG)
///       rnote over SIM: Sends AT commands to set up SIM800L module.
///     <- SIM
///     SIM--
///   @enduml

void SIM_v_Setup(void);

/// @brief Function used for reading messages through SIM800L module
///
/// @pre SIM800L must be configured
/// @post None
/// @param None
///
/// @return None
///
/// @globals None
///
/// @InOutCorelation Function read messages via SIM800L module.
/// @callsequence
///   @startuml "SIM_v_ReceiveMessage.png"
///     title "Sequence diagram for function SIM_v_ReceiveMessage"
///     -> SIM: SIM_v_ReceiveMessage()
///     SIM++
///       SIM -> SIM: v_SendCommand(AT)
///       SIM -> SIM: v_SendCommand(CMGF)
///       SIM -> SIM: v_SendCommand(CNMI)
///     <- SIM
///     SIM--
///   @enduml

void SIM_v_ReceiveMessage(void);

/// @brief Function used for reading coordinates through SIM800L module
///
/// @pre SIM800L must be configured
/// @post None
/// @param None
///
/// @return static uint8_t u_CoordBuf[COORDINATES_BUFFER_LENGTH]
///
/// @globals None
///
/// @InOutCorelation Function read coordinates via SIM800L module.
/// @callsequence
///   @startuml "SIM_p_ReceiveCoordinates.png"
///     title "Sequence diagram for function SIM_p_ReceiveCoordinates"
///     -> SIM: SIM_p_ReceiveCoordinates()
///     SIM++
///       SIM -> SIM: SIM_v_ReceiveMessage()
///     <- SIM://Returns a unit8_t * to a u_CoordBuf//
///     SIM--
///   @enduml

uint8_t * SIM_p_ReceiveCoordinates(void);

/// @brief Function used for ending calls for SIM800L module.
///
/// @pre SIM800L command must be sent first
/// @post None
/// @param None
///
/// @return None
///
/// @globals None
///
/// @InOutCorelation Function ends calls for SIM800L module.
/// @callsequence
///   @startuml "SIM_v_EndCall.png"
///     title "Sequence diagram for function SIM_v_EndCall"
///     -> SIM: SIM_v_EndCall()
///     SIM++
///       SIM -> SIM: v_SendCommand(ATH)
///     <- SIM
///     SIM--
///   @enduml

void SIM_v_EndCall(void);

/// @brief Function used for calling set number via SIM800L module
///
/// @pre SIM800L must be configured
/// @post None
/// @param uint8_t *u_Number
///
/// @return None
///
/// @globals None
///
/// @InOutCorelation Function makes calls via SIM800L module.
/// @callsequence
///   @startuml "SIM_v_CallNumber.png"
///     title "Sequence diagram for function SIM_v_CallNumber"
///     -> SIM: SIM_v_CallNumber(uint8_t *u_Number)
///     SIM++
///       rnote over SIM: Write setup commands for making a call into a message buffer to which number will be added.
///       SIM -> SIM:  v_WriteIntoBuffer(u_Buffer, u_Cnt, u_NumCfg)
///       rnote over SIM: Number characters are also written into same buffer as the configuration message.
///       SIM -> SIM:  v_WriteIntoBuffer(u_Buffer, u_Cnt, u_Number)
///       rnote over SIM: Add ';' character for end of the command.
///       SIM -> SIM:  v_WriteIntoBuffer(u_Buffer, u_Cnt, u_CallCmd)
///       SIM -> SIM: v_SendCommand(AT)
///       UARTM -> SIM: UARTM3_v_SendString(u_Buffer)
///       SIM -> SIM: v_EndOfCommand()
///       rnote over SIM: Calls are made via SIM800L module and rejected after predetermined time frame.
///     <- SIM
///     SIM--
///   @enduml

void SIM_v_CallNumber(uint8_t *u_Number);

/// @brief Function used for calling a number from dictionary via SIM800L module
///
/// @pre SIM800L must be configured and number must be in the dictionary
/// @post None
/// @param e_SIM_KnownCaller e_Caller
///
/// @return None
///
/// @globals t_SIM_Command SIM800L_t_Dictionary[SIM800L_DICTIONARY_LENGTH]
///
/// @InOutCorelation Function makes calls via SIM800L module.
/// @callsequence
///   @startuml "SIM_v_Call.png"
///     title "Sequence diagram for function SIM_v_Call"
///     -> SIM: SIM_v_Call(e_SIM_KnownCaller e_Caller)
///     SIM++
///       loop Goes through SIM800L_t_CallerDictionary elements in order to find known caller
///         opt if known caller is found
///           SIM -> SIM: SIM_v_CallNumber(SIM800L_t_CallerDictionary[u_Cnt].u_Number)
///         end
///       end
///     <- SIM
///     SIM--
///   @enduml

void SIM_v_Call(e_SIM_KnownCaller e_Caller);

/// @brief Function used for sending messages via SIM800L module
///
/// @pre SIM800L must be configured
/// @post None
/// @param uint8_t *u_Message, uint8_t *u_Number
///
/// @return None
///
/// @globals None
///
/// @InOutCorelation Function sends messages via SIM800L module.
/// @callsequence
///   @startuml "SIM_v_SendMessage.png"
///     title "Sequence diagram for function SIM_v_SendMessage"
///     -> SIM: SIM_v_SendMessage(uint8_t *u_Message, uint8_t *u_Number)
///     SIM++
///       rnote over SIM: Write setup commands for sending a message into a message buffer to which number will be added.
///       SIM -> SIM: v_WriteIntoBuffer(u_Buffer, u_Cnt, u_NumCfg)
///       rnote over SIM: Number characters are also written into same buffer as the configuration message.
///       SIM -> SIM:  v_WriteIntoBuffer(u_Buffer, u_Cnt, u_Number)
///       SIM -> SIM: v_SendCommand(AT)
///       SIM -> SIM: v_SendCommand(CMGF)
///       UARTM -> SIM: UARTM3_v_SendString(u_Buffer)
///       SIM -> SIM: v_EndOfCommand()
///       UARTM -> SIM: UARTM3_v_SendString(u_Message)
///       SIM -> SIM: v_EndOfCommand()
///       UARTM -> SIM: UARTM3_v_SendString(ESC)
///       SIM -> SIM: v_EndOfCommand()
///     <- SIM
///     SIM--
///   @enduml

void SIM_v_SendMessage(uint8_t *u_Message, uint8_t *u_Number);

/// @brief Function used for sending messages via SIM800L module
///
/// @pre SIM800L must be configured
/// @post None
/// @param e_SIM_KnownCaller e_Caller
///
/// @return None
///
/// @globals t_SIM_Command SIM800L_t_Dictionary[SIM800L_DICTIONARY_LENGTH]
///
/// @InOutCorelation Function sends coordinates via SIM800L module.
/// @callsequence
///   @startuml "SIM_v_SendCoordinates.png"
///     title "Sequence diagram for function SIM_v_SendCoordinates"
///     -> SIM: SIM_v_SendCoordinates(e_SIM_KnownCaller e_Caller)
///     SIM++
///       loop Goes through SIM800L_t_CallerDictionary in order to find the known caller
///         SIM -> SIM: SIM_v_SendMessage(p_Coordinates, SIM800L_t_CallerDictionary[u_Cnt].u_Number)
///         rnote over SIM: Coordinates are sent via SIM800L module to set number.
///       end
///       loop Goes through elements of u_CoordBuf
///         rnote over SIM: Writes 0 values in all elements in order to clear the previous data
///       end
///       SIM -> SIM: v_WriteIntoBuffer(SIM_u_Buffer, u_Cnt, p_Coordinates)
///     <- SIM
///     SIM--
///   @enduml

void SIM_v_SendCoordinates(e_SIM_KnownCaller e_Caller);

/// @brief Function used for parsing a pointer to a buffer where coordinates read from SIM800L are stored
///
/// @pre SIM800L must be configured
/// @post None
/// @param  None
///
/// @return None
///
/// @globals t_SIM_Flags SIM800L_Flags[SIM800L_STATES]
///
/// @InOutCorelation Function sends pointer to SIM800L_Flags structure to access function flags.
/// @callsequence
///   @startuml "SIM_v_StateMachine.png"
///     title "Sequence diagram for function SIM_v_StateMachine"
///     -> SIM: SIM_v_StateMachine()
///     SIM++
///       SIM -> SIM: SIM_t_Flag()
///       rnote over SIM: e_NextFunction is read from SIM_t_Flag
///       opt if e_PreviosFunction is different from e_NextFunction
///         opt switch IdleFunction
///           rnote over SIM: If other function are done, SIM waits in idle for new function.
///         else else MakeCall
///           MSGM -> SIM: MSGM_p_GetRawMessage()
///           rnote over SIM: Coordinates are stored in the moment when the button is pressed so they won't be rewritten
///           SIM -> SIM:  SIM_v_Call(SIM_module)
///           rnote over SIM: Set e_CurrentFunction as EndCall
///         else else EndCall
///           SIM -> SIM: SIM_v_EndCall()
///           rnote over SIM: Set e_CurrentFunction as SendMessage
///         else else SendMessage
///           SIM -> SIM: SIM_v_SendCoordinates(Aleksandra)
///           rnote over SIM: Sets e_CurrentFunction as ReadMessage
///         else else ReadMessage
///         else else default
///         end
///       end
///       rnote over SIM: e_PreviosFunction gets value of e_NextFunction
///     <- SIM
///     SIM--
///   @enduml

void SIM_v_StateMachine(void);

/// @brief Function used for parsing a pointer to a buffer where coordinates read from SIM800L are stored
///
/// @pre SIM800L must be configured
/// @post None
/// @param  None
///
/// @return t_SIM_Function * SIM800L_t_Functions[SIM800L_STATES]
///
/// @globals t_SIM_Function SIM800L_t_Functions[SIM800L_STATES]
///
/// @InOutCorelation Function sends pointer to t_SIM_Function structure to access function flags.
/// @callsequence
///   @startuml "SIM_p_Function.png"
///     title "Sequence diagram for function SIM_p_Function"
///     -> SIM: SIM_p_Function()
///     SIM++
///       rnote over SIM: Data is read from the structure and sent as a pointer to that structure.
///     <- SIM://Returns a t_SIM_Function * to a SIM800L_t_Functions//
///     SIM--
///   @enduml

t_SIM_Function * SIM_p_Function(void);

/// @brief Function used for parsing semaphore flag value
///
/// @pre None
/// @post None
/// @param  None
///
/// @return boolean b_SemaphoreFlag
///
/// @globals boolean b_SemaphoreFlag
///
/// @InOutCorelation Function sends value of the b_SemaphoreFlag.
/// @callsequence
///   @startuml "SIM_b_GetFlag.png"
///     title "Sequence diagram for function SIM_b_GetFlag"
///     -> SIM: SIM_b_GetFlag()
///     SIM++
///       rnote over SIM: Value is read from the global variable and parsed as return value.
///     <- SIM://Returns a boolean value of the b_SemaphoreFlag//
///     SIM--
///   @enduml

boolean SIM_b_GetFlag(void);

#endif /* SIM_H_ */
