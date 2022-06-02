/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
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

/* USER CODE END Variables */
osThreadId MainTaskHandle;
osThreadId ModBus1TaskHandle;
osThreadId ModBus2TaskHandle;
osThreadId TCP_ServerHandle;
osSemaphoreId InDataTCPHandle;
osSemaphoreId ModBusEndHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void mainTask(void const * argument);
void modbus1Task(void const * argument);
void modbus2Task(void const * argument);
void tcp_server(void const * argument);

extern void MX_LWIP_Init(void);
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
  /* definition and creation of InDataTCP */
  osSemaphoreDef(InDataTCP);
  InDataTCPHandle = osSemaphoreCreate(osSemaphore(InDataTCP), 1);

  /* definition and creation of ModBusEnd */
  osSemaphoreDef(ModBusEnd);
  ModBusEndHandle = osSemaphoreCreate(osSemaphore(ModBusEnd), 1);

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of MainTask */
  osThreadDef(MainTask, mainTask, osPriorityLow, 0, 256);
  MainTaskHandle = osThreadCreate(osThread(MainTask), NULL);

  /* definition and creation of ModBus1Task */
  osThreadDef(ModBus1Task, modbus1Task, osPriorityLow, 0, 256);
  ModBus1TaskHandle = osThreadCreate(osThread(ModBus1Task), NULL);

  /* definition and creation of ModBus2Task */
  osThreadDef(ModBus2Task, modbus2Task, osPriorityLow, 0, 256);
  ModBus2TaskHandle = osThreadCreate(osThread(ModBus2Task), NULL);

  /* definition and creation of TCP_Server */
  osThreadDef(TCP_Server, tcp_server, osPriorityLow, 0, 256);
  TCP_ServerHandle = osThreadCreate(osThread(TCP_Server), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_mainTask */
/**
  * @brief  Function implementing the MainTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_mainTask */
void mainTask(void const * argument)
{
  /* init code for LWIP */
  MX_LWIP_Init();
  /* USER CODE BEGIN mainTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END mainTask */
}

/* USER CODE BEGIN Header_modbus1Task */
/**
* @brief Function implementing the ModBus1Task thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_modbus1Task */
void modbus1Task(void const * argument)
{
  /* USER CODE BEGIN modbus1Task */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END modbus1Task */
}

/* USER CODE BEGIN Header_modbus2Task */
/**
* @brief Function implementing the ModBus2Task thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_modbus2Task */
void modbus2Task(void const * argument)
{
  /* USER CODE BEGIN modbus2Task */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END modbus2Task */
}

/* USER CODE BEGIN Header_tcp_server */
/**
* @brief Function implementing the TCP_Server thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_tcp_server */
void tcp_server(void const * argument)
{
  /* USER CODE BEGIN tcp_server */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END tcp_server */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
