/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "LEDM.h"
#include "MSGM.h"
#include "UARTM.h"
#include "I2C.h"
#include "MCP23017.h"
#include "MONITOR.h"
#include "SIM.h"
#include "WDTIM.h"
#include "tim.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
SemaphoreHandle_t xSemaphore;
/* USER CODE END Variables */
osThreadId TSK_IdleHandle;
osThreadId TSK_LedHandle;
osThreadId TSK_ComHandle;
osThreadId TSK_SIMHandle;
osThreadId TSK_MCP23017Handle;
osSemaphoreId BinSemHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
//osSemaphoreId * FREERTOS_p_GetSemaphore(void);
/* USER CODE END FunctionPrototypes */

void TSK_IdleFun(void const * argument);
void TSK_LedFun(void const * argument);
void TSK_ComFun(void const * argument);
void TSK_SIMFun(void const * argument);
void TSK_MCP23017Fun(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];

void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
  *ppxIdleTaskStackBuffer = &xIdleStack[0];
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
  /* place for user code */
}
/* USER CODE END GET_IDLE_TASK_MEMORY */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* Create the semaphores(s) */
  /* definition and creation of BinSem */
  osSemaphoreDef(BinSem);
  BinSemHandle = osSemaphoreCreate(osSemaphore(BinSem), 1);

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  xSemaphore = xSemaphoreCreateBinary();
  // The semaphore is created in the 'empty' state, meaning the semaphore must first be given
  xSemaphoreGive(xSemaphore);
  xSemaphoreTake(xSemaphore, osWaitForever);
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of TSK_Idle */
  osThreadDef(TSK_Idle, TSK_IdleFun, osPriorityIdle, 0, 128);
  TSK_IdleHandle = osThreadCreate(osThread(TSK_Idle), NULL);

  /* definition and creation of TSK_Led */
  osThreadDef(TSK_Led, TSK_LedFun, osPriorityBelowNormal, 0, 128);
  TSK_LedHandle = osThreadCreate(osThread(TSK_Led), NULL);

  /* definition and creation of TSK_Com */
  osThreadDef(TSK_Com, TSK_ComFun, osPriorityNormal, 0, 128);
  TSK_ComHandle = osThreadCreate(osThread(TSK_Com), NULL);

  /* definition and creation of TSK_SIM */
  osThreadDef(TSK_SIM, TSK_SIMFun, osPriorityRealtime, 0, 128);
  TSK_SIMHandle = osThreadCreate(osThread(TSK_SIM), NULL);

  /* definition and creation of TSK_MCP23017 */
  osThreadDef(TSK_MCP23017, TSK_MCP23017Fun, osPriorityRealtime, 0, 128);
  TSK_MCP23017Handle = osThreadCreate(osThread(TSK_MCP23017), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_TSK_IdleFun */
/**
  * @brief  Function implementing the TSK_Idle thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_TSK_IdleFun */
void TSK_IdleFun(void const * argument)
{
  /* USER CODE BEGIN TSK_IdleFun */
  /* Infinite loop */
  for(;;)
  {
//  osDelay(1);
  }
  /* USER CODE END TSK_IdleFun */
}

/* USER CODE BEGIN Header_TSK_LedFun */
/**
* @brief Function implementing the TSK_Led thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_TSK_LedFun */
void TSK_LedFun(void const * argument)
{
  /* USER CODE BEGIN TSK_LedFun */
	TickType_t xLastWakeTime = xTaskGetTickCount();
  /* Infinite loop */
  for(;;)
  {
	Task_LED_CP_Start();
	LEDM_v_Main();
	vTaskDelayUntil(&xLastWakeTime, (const TickType_t)PERIOD_TSK_LED);
	Task_LED_CP_End();
  }
  /* USER CODE END TSK_LedFun */
}

/* USER CODE BEGIN Header_TSK_ComFun */
/**
* @brief Function implementing the TSK_Com thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_TSK_ComFun */
void TSK_ComFun(void const * argument)
{
  /* USER CODE BEGIN TSK_ComFun */
	TickType_t xLastWakeTime = xTaskGetTickCount();
  /* Infinite loop */
  for(;;)
  {
	// If xTicksToWait is zero, then xSemaphoreTake() will return immediately if the semaphore is not available.
	xSemaphoreTake(xSemaphore, osWaitForever);
	MSGM_v_StateMachine();
	vTaskDelayUntil(&xLastWakeTime, (const TickType_t)PERIOD_TSK_COM);
  }
  /* USER CODE END TSK_ComFun */
}

/* USER CODE BEGIN Header_TSK_SIMFun */
/**
* @brief Function implementing the TSK_SIM thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_TSK_SIMFun */
void TSK_SIMFun(void const * argument)
{
  /* USER CODE BEGIN TSK_SIMFun */
	TickType_t xLastWakeTime = xTaskGetTickCount();
  /* Infinite loop */
  for(;;)
  {
	// Task should enter critical section in order to call the number and receive coordinates without rewriting them
	taskENTER_CRITICAL();
	// Set flag indicates that the SIM module is in any function other than idle
	boolean b_SemFlag = SIM_b_GetFlag();
	do
	{
	  SIM_v_StateMachine();
	  b_SemFlag = SIM_b_GetFlag();
	}
	while(b_SemFlag == b_TRUE);
	xSemaphoreGive(xSemaphore);
	// SIM task should exit critical section when SIM state machine reaches Read function because the next function is idle
	taskEXIT_CRITICAL();
	vTaskDelayUntil(&xLastWakeTime, (const TickType_t)PERIOD_TSK_SIM);
  }
  /* USER CODE END TSK_SIMFun */
}

/* USER CODE BEGIN Header_TSK_MCP23017Fun */
/**
* @brief Function implementing the TSK_MCP23017 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_TSK_MCP23017Fun */
void TSK_MCP23017Fun(void const * argument)
{
  /* USER CODE BEGIN TSK_MCP23017Fun */
	TickType_t xLastWakeTime = xTaskGetTickCount();
  /* Infinite loop */
  for(;;)
  {
	MCP23017_v_TurnLEDviaCoordinates();
	vTaskDelayUntil(&xLastWakeTime, (const TickType_t)PERIOD_TSK_COM);
  }
  /* USER CODE END TSK_MCP23017Fun */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
/* USER CODE END Application */
