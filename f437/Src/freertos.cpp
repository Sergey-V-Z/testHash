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
osThreadId tcpTaskHandle;
osSemaphoreId InDataTCPHandle;
osSemaphoreId ModBusEndHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

// extern "C"
/* USER CODE END FunctionPrototypes */

void TestTask(void const * argument);
void LcdTask(void const * argument);
void i2c_Task(void const * argument);
void ButtonTask(void const * argument);
void TcpTask(void const * argument);

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

  /* definition and creation of tcpTask */
  osThreadDef(tcpTask, TcpTask, osPriorityNormal, 0, 1024);
  tcpTaskHandle = osThreadCreate(osThread(tcpTask), NULL);

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


	uint8_t blink = 0;

	ssd1306_Init();


	//ssd1306_TestAll();
    ssd1306_SetCursor(2, 0);
    ssd1306_WriteString((char*) "Tester hash v1", Font_7x10, White);
    ssd1306_UpdateScreen();

/* Infinite loop */
for (;;) {


	if(counterRefresh >= 500){
		//ssd1306_SetCursor(100, 56);
		if (blink == 0) {
			ssd1306_DrawCircle(100, 56, 4, White);
			//ssd1306_UpdateScreen();
			blink = 1;
			counterRefresh = 0;
		}else {
			ssd1306_DrawCircle(100, 56, 4, Black);
			//ssd1306_UpdateScreen();
			blink = 0;
			counterRefresh = 0;
		}

	}


	ssd1306_UpdateScreen();

	osDelay(84);
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

	//команды i2c
	uint16_t addr = 0x20;
	addr = addr<<1;


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




  /* Infinite loop */
  for(;;)
  {
	  plug = HAL_GPIO_ReadPin(PLUG_GPIO_Port, PLUG_Pin);

	  if (start && plug) {
					status_i2c = HAL_I2C_Master_Transmit(&hi2c1, addr, cmdStart_1, 6, 20);
					osDelay(410);
					status_i2c = HAL_I2C_Master_Receive(&hi2c1, addr, &cmdRead_1[0], 1, 20);
					status_i2c = HAL_I2C_Master_Receive(&hi2c1, addr, &cmdRead_1[1], 1, 20);
					//проверить ответ
					if (cmdRead_1[0] != 0x07 && cmdRead_1[1] != 0x01) {
						start = 0;
						ready = 0;
						ssd1306_Fill(Black); //чистим экран
					    ssd1306_SetCursor(2, 0);
					    ssd1306_WriteString((char*) "error1", Font_7x10, White);
					    //ssd1306_UpdateScreen();
						continue;
					}
					osDelay(710);

					status_i2c = HAL_I2C_Master_Transmit(&hi2c1, addr, cmdStart_2, 6, 20);
					osDelay(110);
					status_i2c = HAL_I2C_Master_Receive(&hi2c1, addr, &cmdRead_2[0], 1, 20);
					status_i2c = HAL_I2C_Master_Receive(&hi2c1, addr, &cmdRead_2[1], 1, 20);
					//проверить ответ
					if (cmdRead_2[0] != 0x06 && cmdRead_2[1] != 0x01) {
						start = 0;
						ready = 0;
						ssd1306_Fill(Black); //чистим экран
					    ssd1306_SetCursor(2, 0);
					    ssd1306_WriteString((char*) "error2", Font_7x10, White);
					    //ssd1306_UpdateScreen();
						continue;
					}
					osDelay(2200);

					status_i2c = HAL_I2C_Master_Transmit(&hi2c1, addr, cmdStart_3, 9, 20);
					osDelay(110);
					status_i2c = HAL_I2C_Master_Receive(&hi2c1, addr, &cmdRead_3[0], 1, 20);
					status_i2c = HAL_I2C_Master_Receive(&hi2c1, addr, &cmdRead_3[1], 1, 20);
					//проверить ответ
					if (cmdRead_3[0] != 0x10 && cmdRead_3[1] != 0x01) {
						start = 0;
						ready = 0;
						ssd1306_Fill(Black); //чистим экран
					    ssd1306_SetCursor(2, 0);
					    ssd1306_WriteString((char*) "error3", Font_7x10, White);
					    //ssd1306_UpdateScreen();
						continue;
					}
					osDelay(710);

					status_i2c = HAL_I2C_Master_Transmit(&hi2c1, addr, cmdStart_4, 7, 20);
					osDelay(30);
					status_i2c = HAL_I2C_Master_Receive(&hi2c1, addr, &cmdRead_4[0], 1, 20);
					status_i2c = HAL_I2C_Master_Receive(&hi2c1, addr, &cmdRead_4[1], 1, 20);
					//проверить ответ
					if (cmdRead_4[0] != 0x15 && cmdRead_4[1] != 0x01) {
						start = 0;
						ready = 0;
						ssd1306_Fill(Black); //чистим экран
					    ssd1306_SetCursor(2, 0);
					    ssd1306_WriteString((char*) "error4", Font_7x10, White);
					    //ssd1306_UpdateScreen();
						continue;
					}


					osDelay(500);
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

							//ожидаем 10 секунд и обновляем пик если за это время был останов то выходим из цикла
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

	RV_BUTTON button1(B1_GPIO_Port, B1_Pin, HIGH_PULL, NORM_OPEN);
	RV_BUTTON button2(B2_GPIO_Port, B2_Pin, HIGH_PULL, NORM_OPEN);
	RV_BUTTON button3(B3_GPIO_Port, B3_Pin, HIGH_PULL, NORM_OPEN);
	uint8_t snum[5];
	uint8_t fan = 0;

	button1.isClick();
	button2.isClick();
	button3.isClick();

	htim4.Instance->CCR3 = 0;
	HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_3);
  /* Infinite loop */
  for(;;)
  {
	button1.tick();
	button2.tick();
	button3.tick();

	if (button1.isClick() ){
		if (start == 0 && plug == GPIO_PIN_SET) {
			start = 1;
			ssd1306_SetCursor(2, 0);
			ssd1306_WriteString((char*) "Test started   ", Font_7x10, White);

		}else if(start >= 1 && plug == GPIO_PIN_SET){
			start = 0;
			ssd1306_SetCursor(2, 0);
			ssd1306_WriteString((char*) "Test stoped     ", Font_7x10, White);

			sprintf((char*)snum, "%-2d", 0);
		    ssd1306_SetCursor(2, 28);
		    ssd1306_WriteString((char*)snum, Font_11x18, White);

		}else {
			start = 0;
			ssd1306_SetCursor(2, 0);
			ssd1306_WriteString((char*) "No plug        ", Font_7x10, White);
		}

	}
	if (button2.isClick() ){
		if (start == 0 && plug == GPIO_PIN_SET) {
			start = 2;
			ssd1306_SetCursor(2, 0);
			ssd1306_WriteString((char*) "Test2 started   ", Font_7x10, White);

		}else if(start >= 1 && plug == GPIO_PIN_SET){
			start = 0;
			ssd1306_SetCursor(2, 0);
			ssd1306_WriteString((char*) "Test stoped     ", Font_7x10, White);

			sprintf((char*)snum, "%-2d", 0);
		    ssd1306_SetCursor(2, 28);
		    ssd1306_WriteString((char*)snum, Font_11x18, White);

		}else {
			start = 0;
			ssd1306_SetCursor(2, 0);
			ssd1306_WriteString((char*) "No plug        ", Font_7x10, White);
		}

	}

	if (button3.isClick() ){
		//HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_3);
		if(fan){
			fan = 0;
			htim4.Instance->CCR3 = ((htim4.Instance->ARR*1000/100)*30)/1000;
		}else{
			fan = 1;
			htim4.Instance->CCR3 = 0;
		}
	}


    osDelay(1);
  }
  /* USER CODE END ButtonTask */
}

/* USER CODE BEGIN Header_TcpTask */
/**
* @brief Function implementing the tcpTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_TcpTask */
void TcpTask(void const * argument)
{
  /* USER CODE BEGIN TcpTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END TcpTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {

	status_uart = HAL_UART_Receive_IT(&huart6, &readASIC[counter_bytes], 1);
	counter_bytes++;
	uartTIM = 0; //сбрасываем таймер до тех пор пока не придут все данные
}

/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
