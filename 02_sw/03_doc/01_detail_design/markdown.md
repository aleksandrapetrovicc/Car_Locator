## Introduction
A two-part hardware device used for locating a car. First part in further text will be referred as a Client, while second part 
will be referred as a Server. Communication between each part is established with calls and text messages. Client requests 
a location from Server by placing a phone call to SIM card assigned to the Server. Server, receiving a phone call from known 
caller (Client) returns its GPS coordinates via text message sent to SIM card assigned to the Client. Client, receiving a text 
message from known sender (Server) with Server's GPS coordinates and based on its own (Client's coordinates) calculates 
in which direction is Server located. Client’s direction will be taken as pointing to North (0 degrees) at all times (room for
future expansion of assignment). Once direction is calculated, LEDs are used to present it in physical form.

## Hardware organization

The client is comprised out of four parts:
1. NUCLEO board – main computing IC, controls entire device,
2. SIM800L – SIM module, used for sending and retrieving necessary data,
3. MCP23017 – GPIO expander, used to drive Diode board,
4. Diode board – contains 8 LEDs, used to present a direction pointing to Server’s location.

The server is comprised out of 3 parts:
1. NUCLEO board – main computing IC, controls entire device,
2. SIM800L – SIM module, used for sending and retrieving necessary data,
3. NEO-6M – GPS module, used for collecting GPS coordinates.

The entire hardware organization is shown on the picture below.

![hworg](pictures\hworg.png "Hardware organization")

Arrangement of the LEDs with colors associated with each of them is shown on the picture below.

![leds](pictures\leds.png "LEDs arrangement")

## Flow chart

UML diagram that represents flow chart of the entire device is shown on the picture below.

![flw](pictures\flw.png "Flow chart")

## Software organization

Due to the complexity of the project, source codes were partitioned into logical segments. Software organization is shown on the picture below.

Legend:
1. orange squares - software components,
2. red rounded rectangles - low level drivers,
3. green parallelograms - hardware components.

![sw](pictures\sw.png "Software organization")

## Tasks

OS used for this project is FreeRTOS. Five tasks of different priorities were generated:
1. TSK_Idle - osPriorityIdle,
2. TSK_Led - osPriorityBelowNormal,
3. TSK_Com - osPriorityNormal,
4. TSK_SIM - osPriorityRealtime,
5. TSK_MCP23017 - osPriorityRealtime.

The role of the tasks is shown in the UML diagram below. 

![tasks](pictures\tasks.png "FreeRTOS tasks")

TSK_Idle does not call any function. It is only used for resting the system when there is no need for interaction with any of the peripherals. Therefore, there is no UML diagram for this task.

TSK_Led is used for debugging. On-board LED should turn on and off each 500 milliseconds. If it is not the case, some serious issue has occured within the system. Since this is a simple function, there is no UML diagram for this task.

First more complicated task is TSK_Com, a task used for handling the message module. This task calls function MSGM_v_StateMachine(), which is implemented as a state machine of MSGM's possible states. Each state and transitions between them are shown in the UML diagram below.

![msgm](pictures\msgm.png "MSGM state machine")

TSK_SIM, task used for handling SIM800L communication calls the function SIM_v_StateMachine(), which is also implemented as a state machine of SIM800L's possible states. Each state and transitions between them are shown in the UML diagram below.

![sim](pictures\sim.png "SIM800L state machine")

TSK_MCP23017, used for handling MCP23017 operations calls the function MCP23017_v_TurnLEDviaCoordinates(). UML diagram of this task is shown on the picture below.

![mcp](pictures\mcp.png "MCP23017 UML diagram")

## Altium Designer

Schematic and layout were created via Altium Designer. They are shown on the pictures below.

![schematic](pictures\schematic.png "Altium Designer schematic")

![layout](pictures\layout.png "Altium Designer layout")

## Entire device

Most of the components were soldered, to decrease bugs caused by loosing contact. The entire device is shown on the picture below.

![device](pictures\device.png "Device")
