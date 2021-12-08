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
#include "string.h"
#include "ssd1306.h"
//#include "ssd1306_tests.h"
#include <stdlib.h>
#include "RV_BUTTON.h"
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

HAL_StatusTypeDef status_i2c;
HAL_StatusTypeDef status_uart;

//для чипов
uint32_t pre_count_ASIC = 0;
uint32_t counter_bytes = 0;
uint8_t readASIC[540] = { 0x00 };

//обшие переменные
uint8_t start = 0; // запуск процедуры проверки
uint8_t ready = 0; // готовнеость питаня

//переменные для таймеров
extern uint32_t uartTIM ;
extern uint8_t status_uartTIM;


/* USER CODE END Variables */
osThreadId testTaskHandle;
osThreadId lcdTaskHandle;
osThreadId I2C_TaskHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

// extern "C"
/* USER CODE END FunctionPrototypes */

void TestTask(void const * argument);
void LCDTask(void const * argument);
void i2c_Task(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
extern "C" void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];

void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer,
		StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize) {
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
  osThreadDef(lcdTask, LCDTask, osPriorityNormal, 0, 256);
  lcdTaskHandle = osThreadCreate(osThread(lcdTask), NULL);

  /* definition and creation of I2C_Task */
  osThreadDef(I2C_Task, i2c_Task, osPriorityNormal, 0, 128);
  I2C_TaskHandle = osThreadCreate(osThread(I2C_Task), NULL);

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
  /* USER CODE BEGIN TestTask */

	uint32_t timeOutUART = 50;
	uint8_t snum[5];
	//команды uart
	uint8_t cmdASIC[7] = { 0x55, 0xAA, 0x52, 0x05, 0x00, 0x00, 0x0A };

	HAL_GPIO_WritePin(RST_ASIC_GPIO_Port, RST_ASIC_Pin, GPIO_PIN_RESET);


	/* Infinite loop */
	for (;;) {
		if (start && ready) {
			counter_bytes = 0;
			HAL_GPIO_WritePin(RST_ASIC_GPIO_Port, RST_ASIC_Pin, GPIO_PIN_RESET);
			osDelay(4);
			HAL_GPIO_WritePin(RST_ASIC_GPIO_Port, RST_ASIC_Pin, GPIO_PIN_SET);
			osDelay(1200);

			//цикл опроса
			while (start) {

				//сброс асиков
				HAL_GPIO_WritePin(RST_ASIC_GPIO_Port, RST_ASIC_Pin, GPIO_PIN_RESET);
				osDelay(500);
				HAL_GPIO_WritePin(RST_ASIC_GPIO_Port, RST_ASIC_Pin, GPIO_PIN_SET);
				osDelay(500);

				status_uart = HAL_UART_Receive_IT(&huart6, readASIC, 1);
				status_uart = HAL_UART_Transmit(&huart1, cmdASIC, 7, 20);

				//запускаем таймер
				status_uartTIM = 1;
				// ожидать таймаута по уарту
				while(uartTIM < timeOutUART){
					osDelay(1);
				}
				uartTIM = 0;
				status_uartTIM = 0;

				// все данные пришли отключаем уарт
				HAL_UART_AbortReceive(&huart6);

				//проверить пришедшие данные сравненией с шаблоном

				//посчитать количесво асиков

				pre_count_ASIC = counter_bytes / 9;
				itoa(pre_count_ASIC, (char*)snum, 10);

			    ssd1306_SetCursor(2, 0);
			    ssd1306_WriteString((char*)snum, Font_16x26, White);
			    ssd1306_UpdateScreen();

				counter_bytes = 0;
				memset(readASIC, 0, sizeof readASIC);

				osDelay(200);

			}

	}
	//osDelay(1);
}
  /* USER CODE END TestTask */
}

/* USER CODE BEGIN Header_LCDTask */
/**
 * @brief Function implementing the lcdTask thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_LCDTask */
void LCDTask(void const * argument)
{
  /* USER CODE BEGIN LCDTask */

	RV_BUTTON button1(B1_GPIO_Port, B1_Pin, HIGH_PULL, NORM_OPEN);
	RV_BUTTON button2(B2_GPIO_Port, B2_Pin, HIGH_PULL, NORM_OPEN);
	RV_BUTTON button3(B3_GPIO_Port, B3_Pin, HIGH_PULL, NORM_OPEN);

	button1.isClick();
	button2.isClick();
	button3.isClick();


	//ssd1306_TestAll();
    ssd1306_SetCursor(2, 28);
    ssd1306_WriteString("Start", Font_16x26, White);
    ssd1306_UpdateScreen();

/* Infinite loop */
for (;;) {
	button1.tick();
	button2.tick();
	button3.tick();

	if (button1.isClick() ){
		if (start == 0) {
			start = 1;
		}else {
			start = 0;
		}

	}

	osDelay(1);
}
  /* USER CODE END LCDTask */
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

	//команды i2c
	uint16_t addr = 0x20;
	addr = addr<<1;

	GPIO_PinState plug = GPIO_PIN_RESET;

	uint8_t cmdStart_1[6] = { 0x55, 0xAA, 0x04, 0x07, 0x00, 0x0B };
	uint8_t cmdRead_1[2] = { 0x00 };// 0x07 0x01

	uint8_t cmdStart_2[6] = { 0x55, 0xAA, 0x04, 0x06, 0x00, 0x0A };
	uint8_t cmdRead_2[2] = { 0x00 };// 0x06 0x01

	uint8_t cmdStart_3[9] = { 0x55, 0xAA, 0x07, 0x10, 0x19, 0x00, 0x00, 0x00, 0x30 };
	uint8_t cmdRead_3[2] = { 0x00 };// 0x10 0x01

	uint8_t cmdStart_4[7] = { 0x55, 0xAA, 0x05, 0x15, 0x01, 0x00, 0x1B };
	uint8_t cmdRead_4[2] = { 0x00 };// 0x15 0x01

	uint8_t cmdStop[7] = { 0x55, 0xAA, 0x05, 0x15, 0x00, 0x00, 0x1A };
	uint8_t cmdRead_stop[2] = { 0x00 };// 0x15 0x01

	uint8_t cmdRefresh[6] = { 0x55, 0xAA, 0x04, 0x16, 0x00, 0x1A };
	uint8_t cmdRead_Refresh[6] = { 0x00 };// 0x06 0x16 0x01 0x00 0x00 0x1D


	HAL_GPIO_WritePin(A0_GPIO_Port, A0_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(A1_GPIO_Port, A1_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(A2_GPIO_Port, A2_Pin, GPIO_PIN_RESET);


	plug = HAL_GPIO_ReadPin(PLUG_GPIO_Port, PLUG_Pin);

  /* Infinite loop */
  for(;;)
  {

	  if (start && plug) {
					status_i2c = HAL_I2C_Master_Transmit(&hi2c1, addr, cmdStart_1, 6, 20);
					osDelay(200);
					status_i2c = HAL_I2C_Master_Receive(&hi2c1, addr, &cmdRead_1[0], 1, 20);
					status_i2c = HAL_I2C_Master_Receive(&hi2c1, addr, &cmdRead_1[1], 1, 20);
					osDelay(300);

					status_i2c = HAL_I2C_Master_Transmit(&hi2c1, addr, cmdStart_2, 6, 20);
					osDelay(200);
					status_i2c = HAL_I2C_Master_Receive(&hi2c1, addr, &cmdRead_2[0], 1, 20);
					status_i2c = HAL_I2C_Master_Receive(&hi2c1, addr, &cmdRead_2[1], 1, 20);
					osDelay(1000);

					status_i2c = HAL_I2C_Master_Transmit(&hi2c1, addr, cmdStart_3, 9, 20);
					osDelay(100);
					status_i2c = HAL_I2C_Master_Receive(&hi2c1, addr, &cmdRead_3[0], 1, 20);
					status_i2c = HAL_I2C_Master_Receive(&hi2c1, addr, &cmdRead_3[1], 1, 20);
					osDelay(1000);

					status_i2c = HAL_I2C_Master_Transmit(&hi2c1, addr, cmdStart_4, 7, 20);
					osDelay(710);
					status_i2c = HAL_I2C_Master_Receive(&hi2c1, addr, &cmdRead_4[0], 1, 20);
					status_i2c = HAL_I2C_Master_Receive(&hi2c1, addr, &cmdRead_4[1], 1, 20);


					osDelay(1000);
					//выдать флаг готовности питания
					ready = 1;

					osDelay(3000);
					//обновляем пик
					status_i2c = HAL_I2C_Master_Transmit(&hi2c1, addr, cmdRefresh, 6, 20);
					status_i2c = HAL_I2C_Master_Receive(&hi2c1, addr, cmdRead_Refresh, 6, 20);

					// запустить цикл обновления до остановки пользователем
					while(start){
							//обновляем пик
							status_i2c = HAL_I2C_Master_Transmit(&hi2c1, addr, cmdRefresh, 6, 20);
							osDelay(10);
							status_i2c = HAL_I2C_Master_Receive(&hi2c1, addr, &cmdRead_Refresh[0], 1, 20);
							status_i2c = HAL_I2C_Master_Receive(&hi2c1, addr, &cmdRead_Refresh[1], 1, 20);
							status_i2c = HAL_I2C_Master_Receive(&hi2c1, addr, &cmdRead_Refresh[2], 1, 20);
							status_i2c = HAL_I2C_Master_Receive(&hi2c1, addr, &cmdRead_Refresh[3], 1, 20);
							status_i2c = HAL_I2C_Master_Receive(&hi2c1, addr, &cmdRead_Refresh[4], 1, 20);
							status_i2c = HAL_I2C_Master_Receive(&hi2c1, addr, &cmdRead_Refresh[5], 1, 20);

							for (int var = 0; var < 10000; ++var) {
								if(!start){
									break;
								}
								osDelay(1);
							}

							if(!start){
									break;
							}

					}

					// выключение питания
					status_i2c = HAL_I2C_Master_Transmit(&hi2c1, addr, cmdStop, 7, 20);
					osDelay(20);
					status_i2c = HAL_I2C_Master_Receive(&hi2c1, addr, &cmdRead_stop[0], 1, 20);
					status_i2c = HAL_I2C_Master_Receive(&hi2c1, addr, &cmdRead_stop[1], 1, 20);
					//выдать флаг отключения питания
					ready = 0;
	  }

  }

  /* USER CODE END i2c_Task */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {

	status_uart = HAL_UART_Receive_IT(&huart6, &readASIC[counter_bytes], 1);
	counter_bytes++;
	uartTIM = 0; //сбрасываем таймер до тех пор пока не придут все данные
}

/* USER CODE END Application */
