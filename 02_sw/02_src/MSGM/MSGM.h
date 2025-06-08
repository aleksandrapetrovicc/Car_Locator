/// @file MSGM.h
/// @brief Header file used to manipulate different types of messages
/// @author Aleksandra Petrovic
#ifndef MSGM_H_
#define MSGM_H_

#include "UARTM.h"

/// Used to define number of words in a dictionary
#define MSGM_DICTIONARY_LENGTH (2u)
/// Used as a number of slots for temporary buffer used to find words
#define MSGM_MESSAGE_BUFFER_LENGTH (10u)
/// Used as a last slot of a temporary message buffer
#define MSGM_MESSAGE_BUFFER_SLOT (9u)
/// Defines the size of a ring buffer used to receive the data from the GPS module
#define MSGM_RING_BUFFER_LENGTH (512u)
/// Used for size of an array that stores latitude and longitude
#define COORDINATES_LENGTH (20u)
/// Used as a size of a temporary buffer that stores coordinates from interrupt service routine
#define COORDINATES_BUFFER_LENGTH (50u)

/// This enum is used for different types of messages sent to modules from UART
typedef enum {
  DEFAULT_MSG,  ///< Default message in a buffer if no valid word is stored
  LED_ON____,   ///< Message of a buffer that signalizes that LED is ON
  LED_OFF___    ///< Message of a buffer that signalizes that LED is OFF
} e_Message;

/// This enum is used for different IDs and the modules where messages should be sent
typedef enum
{
  DEFAULT_ID,  ///< Default ID in a buffer if there is no specified module for an upcoming message
  LEDM,        ///< ID of messages used in LED module
  SIM          ///< ID of messages used in SIM 800L module
} e_ReceiverID;

typedef enum
{
  b_TRUE,  ///< Value used when the condition is true
  b_FALSE  ///< Value used when the condition is false
} boolean;

typedef enum
{
  Idle_State,     ///< State in which the system is waiting for a '$' sign
  GPS_State,      ///< State in which coordinates from the module are detected and to be extracted in the next state
  Message_State,  ///< State in which message is being transfered to temporary buffer until the '*' sign
  Transfer_State,  ///< State in which the extraction of relevant data is parsed from the string
} e_SystemState;

/// This structure is used as a buffer in a function that sorts messages
typedef struct {
  uint8_t * u_RawMessage;  ///< Field used to receive a raw message from UART manager
  e_ReceiverID e_ID;       ///< Field used to store a message unique identification number
  e_Message e_Msg;         ///< Field used to store a message into the buffer
  uint8_t u_Length;        ///< Field used to store a length of a message
} t_MessageElement;

/// Structure used as a ring buffer
typedef struct {
  uint8_t  u_buffer[MSGM_RING_BUFFER_LENGTH]; ///< Ring buffer that stores the messages from GPS module
  uint16_t u_head;                            ///< Variable used as a tracker of data being stored into a ring buffer
  uint16_t u_tail;                            ///< Variable used as a tracker of data being popped out of a ring buffer
  uint16_t u_count;                           ///< Tracks the difference between two trackers to check if data is being manipulated properly
} t_CircularBuffer;

typedef struct {
  uint8_t a_Latitude [COORDINATES_LENGTH];    ///< Array with size of 20 to store latitude from GPS
  uint8_t a_Longitude[COORDINATES_LENGTH];    ///< Array with size of 20 to store longitude from GPS
  uint8_t u_LatDirection;                     ///< Direction for a latitude coordinate
  uint8_t u_LonDirection;                     ///< Direction for a longitude coordinate
} t_CoordinatesStructure;

typedef enum
{
  RING_BUFFER1,                               ///< Enum of a ring buffers in a system
  NUM_OF_RING_BUFFERS                         ///< Number of buffers in a system
} e_RingBuffers;

t_CoordinatesStructure * MSGM_p_GetCoordinates();

/// @brief Function used to sort a message taken from UART serial communication
///
/// @pre Raw message must be stored into a temporary buffer
/// @post None
/// @param uint8_t *UARTM_a_array
///
/// @return None
///
/// @globals UARTM_a_array
///
/// @InOutCorelation Function recieves a raw message and sorts it based on module ID
/// @callsequence
///   @startuml "MSGM_v_Sort.png"
///     title "Sequence diagram for function MSGM_v_Sort"
///     -> MSGM: MSGM_v_Sort(...)
///     MSGM++
///     loop waits for a special character to reach a temporary buffer
///       rnote over MSGM: goes through slots of a temporary buffer
///     end
///     loop goes through a dictionary of valid messages
///       opt if length of a valid word is the same as the length of a raw message
///         loop reads a word character by character
///           opt checks if the raw message is different to the dictionary valid word
///             rnote over MSGM: block the word and break out of the loop
///           else else
///             rnote over MSGM: approve of the word
///           end
///         end
///       end
///       opt if the word is valid and ready to be stored
///         opt if the number of free buffer slots reached it's limit
///           rnote over MSGM: Move the buffer slot until the limit is reached, then reset
///         else else
///           rnote over MSGM: Move the buffer slot
///         end
///       end
///     end
///       MSGM -> UARTM: UARTM_v_ClearBuffer()
///       MSGM--
///       UARTM++
///       rnote over UARTM: Clears global buffer for raw message
///       UARTM -> MSGM
///       UARTM--
///       MSGM++
///       <- MSGM
///       MSGM--
///   @enduml
void MSGM_v_Sort(uint8_t *UARTM_a_array);

/// @brief Function used to clear the buffer after sorting the messages
///
/// @pre Buffer is not empty
/// @post None
/// @param uint8_t u_Index used to navigate the slot for deletion
///
/// @return None
///
/// @globals None
///
/// @InOutCorelation Function detects the buffer slot and replaces message with the default
/// @callsequence
///   @startuml "MSGM_v_ClearMessage.png"
///     title "Sequence diagram for function MSGM_v_ClearMessage"
///     -> MSGM: MSGM_v_ClearMessage()
///     MSGM++
///         rnote over MSGM: Deletes raw message and replaces message and id template with defaults
///     <- MSGM
///     MSGM--
///   @enduml
void MSGM_v_ClearMessage(uint8_t index);

/// @brief Function used to retrieve the messages based on their ID number
///
/// @pre Buffer is not empty
/// @post Returns the message that is identical to predefined ID number
/// @param e_ReceiverID e_ID used to send an ID number
///
/// @return e_MessageReturn with the valid message from the dictionary
///
/// @globals None
///
/// @InOutCorelation Function that takes and ID and sends back messages that correlate with ID
/// @callsequence
///   @startuml "MSGM_e_MessageRetrieve.png"
///     title "Sequence diagram for function MSGM_messageRetrieve"
///     -> MSGM: MSGM_e_MessageRetrieve()
///     MSGM++
///         loop goes through predefined dictionary
///           opt if ID of a stored message is identical to modules ID
///             rnote over MSGM: Returns the message and clears the buffer slot
///             MSGM -> MSGM: MSGM_v_ClearMessage(...)
///           end
///         end
///     <- MSGM: Returns e_Message with the valid word and ID
///        MSGM--
///   @enduml
e_Message MSGM_e_MessageRetrieve(e_ReceiverID e_ID);

/// @brief Function used to write messages into the ring buffer
///
/// @pre None
/// @post The message is written into the ring buffer
/// @param e_RingBuffers e_BufferID to send an ID of adequate buffer, uint8_t UARTM_u_data to send the data into the buffer
///
/// @return None
///
/// @globals t_RingBuffer structure used as a ring buffer
///
/// @InOutCorelation Function receives the data from interrupt routine and writes it into the ring buffer
/// @callsequence
///   @startuml "MSGM_u_CircularBufferPush.png"
///     title "Sequence diagram for function MSGM_u_CircularBufferPush"
///     -> MSGM: MSGM_u_CircularBufferPush()
///     MSGM++
///         opt if Correct ring buffer is selected to store data
///         end
///         opt if Ring buffer is not full
///         end
///         opt if Reading pointer is ahead of a write pointer
///         end
///         rnote over MSGM: Stores the data inside of a ring buffer,\n checks if buffer is full and moves to next slot
///     <- MSGM
///        MSGM--
///   @enduml
uint8_t MSGM_u_CircularBufferPush (e_RingBuffers e_BufferID, uint8_t UARTM_u_data);

/// @brief Function used to read messages from the ring buffer
///
/// @pre Buffer has the data that is not yet processed
/// @post The content is read from the ring buffer and cleared out of it
/// @param e_RingBuffers e_BufferID to send an ID of adequate buffer
///
/// @return Returns the data of type uint8_t that is being read
///
/// @globals t_RingBuffer structure used as a ring buffer
///
/// @InOutCorelation Function reads the data from the ring buffer and clears it
/// @callsequence
///   @startuml "MSGM_u_CircularBufferPop.png"
///     title "Sequence diagram for function MSGM_u_CircularBufferPop"
///     -> MSGM: MSGM_u_CircularBufferPop()
///     MSGM++
///         opt if Correct ring buffer is selected to store data
///         end
///         opt if All of the data is popped
///         end
///         rnote over MSGM: Pops the data out of the ring buffer,\n moves to the next slot and clears the previous one
///     <- MSGM: Returns uint8_t with the data that was read most recently
///        MSGM--
///   @enduml
uint8_t MSGM_u_CircularBufferPop (e_RingBuffers e_BufferID);

/// @brief Function used to check if the ring buffer is empty
///
/// @pre None
/// @post None
/// @param e_RingBuffers e_BufferID to check if the correct buffer is empty
///
/// @return Boolean value that is true if the buffer is empty
///
/// @globals t_RingBuffer structure with write and read pointers
///
/// @InOutCorelation Function checks if the ring buffer is empty and returns the status of it
/// @callsequence
///   @startuml "MSGM_b_CircularBufferIsEmpty.png"
///     title "Sequence diagram for function MSGM_b_CircularBufferIsEmpty"
///     -> MSGM: MSGM_b_CircularBufferIsEmpty()
///     MSGM++
///     <- MSGM: Returns boolean which is true if the buffer in question is empty
///     MSGM--
///   @enduml
boolean MSGM_b_CircularBufferIsEmpty (e_RingBuffers e_BufferID);

/// @brief Function used to go through a state machine and parse the part of a string for coordinates
///
/// @pre Raw message must be stored into a ring buffer
/// @post None
/// @param None
///
/// @return None
///
/// @globals e_SystemState type of a variable to set a state machine in an initial state,
///          uint8_t data variable used to manipulate with the data from the ring buffer,
///          t_RingBuffer structure that represents a ring buffer
///
/// @InOutCorelation Function receives the data from the ring buffer through the pop function and parses coordinates from it
/// @callsequence
///   @startuml "MSGM_v_StateMachine.png"
///     title "Sequence diagram for function MSGM_v_StateMachine"
///     -> MSGM: MSGM_v_StateMachine(...)
///     MSGM++
///     loop goes through the loop for a specific number of times to catch as many characters through interrupt routine
///       rnote over MSGM: Goes through state machine cases
///         opt switch Idle state
///           opt if Checks if the buffer is empty
///              MSGM -> MSGM: MSGM_b_CircularBufferIsEmpty(...)
///           else else
///              MSGM -> MSGM: MSGM_u_CircularBufferPop(...)
///              opt if Checks if the data is equal to '$' sign
///                rnote over MSGM: Goes to GPS state
///              end
///           end
///     else else GPS state
///         MSGM -> MSGM: MSGM_u_CircularBufferPop(...)
///         opt if Data is equal to the sign 'L'
///           MSGM -> MSGM: MSGM_u_CircularBufferPop(...)
///           opt if Data is equal to the sign 'L'
///             rnote over MSGM: Goes to Message state
///           else else
///             rnote over MSGM: Goes to Idle state
///           end
///         end
///     else else Message state
///         MSGM -> MSGM: MSGM_u_CircularBufferPop(...)
///         opt if Data is equal to the sign '*'
///           rnote over MSGM: u_RawEnd gets value of current index
///           rnote over MSGM: Goes to Transfer State
///           else else
///             opt if Temporary buffer for data is full
///               opt if System doesn't pop out of ring buffer while it's empty
///                 rnote over MSGM: Goes to Transfer state
///               end
///             else else
///               rnote over MSGM: Goes to Idle state and resets the position of a temporary buffer
///             end
///         end
///     else else Transfer state
///        loop While counter reacher u_RawEnd
///          rnote over MSGM: Writes values of a_TempBuffer in u_RawMessageBuffer
///        end
///        MSGM -> MSGM: MSGM_t_GetCoordinates(...)
///        rnote over MSGM: Resets the position of temporary buffer and goes to Idle state
///      end
///     end
///       <- MSGM
///       MSGM--
///   @enduml
void MSGM_v_StateMachine ();

/// @brief Function used to retrieve the coordinates
///
/// @pre String with coordinates information is parsed and stored into the temporary buffer
/// @post Stores the data into the coordinates structure
/// @param uint8_t *TempBuffer that points to the buffer with the stored string with coordinates
///
/// @return None
///
/// @globals t_Coordinates structure in which important data is stored, temporary buffer array from which the data is stored
///
/// @InOutCorelation Function receives the data from temporary buffer, parses it and sends parts to structure
/// @callsequence
///   @startuml "MSGM_t_GetCoordinates.png"
///     title "Sequence diagram for function MSGM_t_GetCoordinates"
///     -> MSGM: MSGM_t_GetCoordinates()
///     MSGM++
///         loop Goes through the temporary buffer until it reaches ',' sign
///           rnote over MSGM: Stores the latitude coordinate from the buffer to structure field
///         end
///           rnote over MSGM: Stores the latitude direction from the buffer to structure field
///          loop Goes through the temporary buffer again until it reaches ',' sign
///             rnote over MSGM: Stores the longitude coordinate from the buffer to structure field
///           end
///             rnote over MSGM: Stores the longitude direction from the buffer to structure field
///     <- MSGM
///        MSGM--
///   @enduml
void MSGM_t_GetCoordinates (uint8_t* TempBuffer);

/// @brief Function used for parsing a pointer to a buffer where processed message gotten through UART is written
///
/// @pre UART must be configured
/// @post None
/// @param  None
///
/// @return t_CoordinatesStructure MSGM_t_Coordinates
///
/// @globals t_CoordinatesStructure MSGM_t_Coordinates
///
/// @InOutCorelation Function sends pointer to MSGM_t_Coordinates buffer.
/// @callsequence
///   @startuml "MSGM_p_GetCoordinates.png"
///     title "Sequence diagram for function MSGM_p_GetCoordinates"
///     -> MSGM: MSGM_p_GetCoordinates()
///     MSGM++
///     rnote over MSGM: Coordinates are read from the buffer and sent as a pointer to a buffer.
///     MSGM--
///     <- MSGM://Returns a t_CoordinatesStructure * to a MSGM_t_Coordinates//
///   @enduml

t_CoordinatesStructure * MSGM_p_GetCoordinates(void);

/// @brief Function used for parsing a pointer to a buffer where raw message gotten through UART is written
///
/// @pre UART must be configured
/// @post None
/// @param  None
///
/// @return uint8_t * u_RawMessageBuffer
///
/// @globals uint8_t u_RawMessageBuffer[50u]
///
/// @InOutCorelation Function sends pointer to u_RawMessageBuffer buffer.
/// @callsequence
///   @startuml "MSGM_p_GetRawMessage.png"
///     title "Sequence diagram for function MSGM_p_GetRawMessage"
///     -> MSGM: MSGM_p_GetRawMessage()
///     MSGM++
///       rnote over MSGM: Coordinates are read from the buffer and sent as a pointer to a buffer.
///     MSGM--
///     <- MSGM://Returns a unit8_t * to a u_RawMessageBuffer//
///   @enduml

uint8_t * MSGM_p_GetRawMessage(void);

#endif /* MSGM_H_ */
