/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * File Name          : freertos.c
 * Description        : Code for freertos applications
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
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
#include "stdio.h"
#include "string.h"
#include "ssd1306.h"
//#include "ssd1306_tests.h"
#include <stdlib.h>
#include "RV_BUTTON.h"
#include "lwip.h"
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
extern I2C_HandleTypeDef hi2c1;
extern I2C_HandleTypeDef hi2c3;
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart6;
extern TIM_HandleTypeDef htim14;
extern TIM_HandleTypeDef htim4;

HAL_StatusTypeDef status_i2c;
HAL_StatusTypeDef status_uart;

//для чипов
uint32_t pre_count_ASIC = 0;
uint32_t counter_bytes = 0;
uint8_t readASIC[540] = { 0x00 };

//обшие переменные
uint8_t start = 0; // запуск процедуры проверки
uint8_t ready = 0; // готовнеость питаня
GPIO_PinState plug = GPIO_PIN_RESET; //присутствие платы

//переменные для таймеров
extern uint32_t uartTIM ;
extern uint8_t status_uartTIM;
extern uint32_t counterRefresh;


/* USER CODE END Variables */
osThreadId testTaskHandle;
osThreadId lcdTaskHandle;
osThreadId I2C_TaskHandle;
osThreadId buttonTaskHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

// extern "C"
/* USER CODE END FunctionPrototypes */

void TestTask(void const * argument);
void LcdTask(void const * argument);
void i2c_Task(void const * argument);
void ButtonTask(void const * argument);

extern void MX_LWIP_Init(void);
void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
extern "C" void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

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
  /* definition and creation of testTask */
  osThreadDef(testTask, TestTask, osPriorityNormal, 0, 256);
  testTaskHandle = osThreadCreate(osThread(testTask), NULL);

  /* definition and creation of lcdTask */
  osThreadDef(lcdTask, LcdTask, osPriorityNormal, 0, 512);
  lcdTaskHandle = osThreadCreate(osThread(lcdTask), NULL);

  /* definition and creation of I2C_Task */
  osThreadDef(I2C_Task, i2c_Task, osPriorityNormal, 0, 256);
  I2C_TaskHandle = osThreadCreate(osThread(I2C_Task), NULL);

  /* definition and creation of buttonTask */
  osThreadDef(buttonTask, ButtonTask, osPriorityNormal, 0, 128);
  buttonTaskHandle = osThreadCreate(osThread(buttonTask), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_TestTask */
/**
  * @brief  Function implementing the testTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_TestTask */
void TestTask(void const * argument)
{
  /* init code for LWIP */
  MX_LWIP_Init();
  /* USER CODE BEGIN TestTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END TestTask */
}

/* USER CODE BEGIN Header_LcdTask */
/**
* @brief Function implementing the lcdTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_LcdTask */
void LcdTask(void const * argument)
{
  /* USER CODE BEGIN LcdTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END LcdTask */
}

/* USER CODE BEGIN Header_i2c_Task */
/**
* @brief Function implementing the I2C_Task thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_i2c_Task */
void i2c_Task(void const * argument)
{
  /* USER CODE BEGIN i2c_Task */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END i2c_Task */
}

/* USER CODE BEGIN Header_ButtonTask */
/**
* @brief Function implementing the buttonTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_ButtonTask */
void ButtonTask(void const * argument)
{
  /* USER CODE BEGIN ButtonTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END ButtonTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {

	status_uart = HAL_UART_Receive_IT(&huart6, &readASIC[counter_bytes], 1);
	counter_bytes++;
	uartTIM = 0; //сбрасываем таймер до тех пор пока не придут все данные
}

/* USER CODE END Application */
