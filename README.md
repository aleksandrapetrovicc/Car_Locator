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


![hworg](https://github.com/user-attachments/assets/9da9f618-0dbe-4d6a-9449-d3de08d31d5f)


Arrangement of the LEDs with colors associated with each of them is shown on the picture below.


![leds](https://github.com/user-attachments/assets/39b7a24e-aadb-4b8b-a31e-fe54860ca5ac)


## Flow chart

UML diagram that represents flow chart of the entire device is shown on the picture below.


![flw](https://github.com/user-attachments/assets/3a41fd94-65e5-4a40-af3b-50212bc45108)


## Software organization

Due to the complexity of the project, source codes were partitioned into logical segments. Software organization is shown on the picture below.

Legend:
1. orange squares - software components,
2. red rounded rectangles - low level drivers,
3. green parallelograms - hardware components.


![sw](https://github.com/user-attachments/assets/f9e8405e-052b-4112-a281-3aeb4d4fcf9f)


## Tasks

OS used for this project is FreeRTOS. Five tasks of different priorities were generated:
1. TSK_Idle - osPriorityIdle,
2. TSK_Led - osPriorityBelowNormal,
3. TSK_Com - osPriorityNormal,
4. TSK_SIM - osPriorityRealtime,
5. TSK_MCP23017 - osPriorityRealtime.

The role of the tasks is shown in the UML diagram below. 


![tasks](https://github.com/user-attachments/assets/1aafbd58-f960-4574-8738-26b2988b6a41)


TSK_Idle does not call any function. It is only used for resting the system when there is no need for interaction with any of the peripherals. Therefore, there is no UML diagram for this task.

TSK_Led is used for debugging. On-board LED should turn on and off each 500 milliseconds. If it is not the case, some serious issue has occured within the system. Since this is a simple function, there is no UML diagram for this task.

First more complicated task is TSK_Com, a task used for handling the message module. This task calls function MSGM_v_StateMachine(), which is implemented as a state machine of MSGM's possible states. Each state and transitions between them are shown in the UML diagram below.


![msgm](https://github.com/user-attachments/assets/d7a4cbf9-81ae-494e-a4c3-d777a3803162)


TSK_SIM, task used for handling SIM800L communication calls the function SIM_v_StateMachine(), which is also implemented as a state machine of SIM800L's possible states. Each state and transitions between them are shown in the UML diagram below.


![sim](https://github.com/user-attachments/assets/a471fd4d-e2e7-40a9-bf8d-7ecc7da346e7)


TSK_MCP23017, used for handling MCP23017 operations calls the function MCP23017_v_TurnLEDviaCoordinates(). UML diagram of this task is shown on the picture below.


![mcp](https://github.com/user-attachments/assets/d105d28c-0a36-49e5-b15c-3f55b45eb649)


## Altium Designer

Schematic and layout were created via Altium Designer. They are shown on the pictures below.


![schematic](https://github.com/user-attachments/assets/ffc53dc1-c29f-4671-91ef-5cdfe1b4026b)


![layout](https://github.com/user-attachments/assets/abd7191e-4460-47b8-a0db-af7e4878d128)


## Entire device

Most of the components were soldered, to decrease bugs caused by loosing contact. The entire device is shown on the picture below.

![device](https://github.com/user-attachments/assets/e126c983-d6fa-4423-86af-f9058b299483)

