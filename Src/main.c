
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * Copyright (c) 2018 STMicroelectronics International N.V. 
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f4xx_hal.h"
#include "cmsis_os.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;

osThreadId Task1Handle;
osThreadId Task2Handle;
osThreadId Task3Handle;
osThreadId Task4Handle;
osMailQId mailqueueHandle;
osMutexId ADCmutexHandle;

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/
ADC_ChannelConfTypeDef sConfig = {ADC_CHANNEL_0, 1, ADC_SAMPLETIME_3CYCLES};

typedef struct
{
  uint32_t source;
  float value;
  float average;
  float variance;
  uint32_t timestamp;
} data;

/*
	polje source identificira Task, polje value storira trenutnu vrijednost signala, polje average i
	variance srednju vrijednost i varijansu signala a polje timestamp vrijeme mjerenja odnosno
	sracunavanja vrijednosti signala.
*/

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ADC1_Init(void);
void StarttTask1(void const *argument);
void StartTask2(void const *argument);
void StartTask3(void const *argument);
void StartTask4(void const *argument);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

/* USER CODE END PFP */

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  *
  * @retval None
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration----------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_ADC1_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Create the mutex(es) */
  /* definition and creation of ADCmutex */
  osMutexDef(ADCmutex);
  ADCmutexHandle = osMutexCreate(osMutex(ADCmutex));

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the thread(s) */
  /* definition and creation of Task1 */
  osThreadDef(Task1, StarttTask1, osPriorityHigh, 0, 128);
  Task1Handle = osThreadCreate(osThread(Task1), NULL);

  /* definition and creation of Task2 */
  osThreadDef(Task2, StartTask2, osPriorityAboveNormal, 0, 128);
  Task2Handle = osThreadCreate(osThread(Task2), NULL);

  /* definition and creation of Task3 */
  osThreadDef(Task3, StartTask3, osPriorityNormal, 0, 128);
  Task3Handle = osThreadCreate(osThread(Task3), NULL);

  /* definition and creation of Task4 */
  osThreadDef(Task4, StartTask4, osPriorityBelowNormal, 0, 128);
  Task4Handle = osThreadCreate(osThread(Task4), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* Create the queue(s) */
  /* definition and creation of mailqueue */
  osMailQDef(mailqueue, 32, data);
  mailqueueHandle = osMailCreate(osMailQ(mailqueue), NULL);
  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;

  /**Configure the main internal regulator output voltage 
    */
  __HAL_RCC_PWR_CLK_ENABLE();

  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

  /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = 16;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  /**Configure the Systick interrupt time 
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq() / 1000);

  /**Configure the Systick 
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 15, 0);
}

/* ADC1 init function */
static void MX_ADC1_Init(void)
{

  ADC_ChannelConfTypeDef sConfig;

  /**Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion) 
    */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.ScanConvMode = DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.DMAContinuousRequests = DISABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  /**Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time. 
    */
  sConfig.Channel = ADC_CHANNEL_0;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }
}

/** Pinout Configuration
*/
static void MX_GPIO_Init(void)
{

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/* StarttTask1 function */
void StarttTask1(void const *argument)
{
  /* USER CODE BEGIN 5 */
  uint32_t startTime, endTime;
  static uint32_t numberOfSamples = 0;
  static float measuredValues[20];
  float average;
  float variance;
  data *message;
  /* Infinite loop */
  for (;;)
  {
    startTime = osKernelSysTick();

    osMutexWait(ADCmutexHandle, osWaitForever); // Start of the critical section -> use mutex
    sConfig.Channel = ADC_CHANNEL_0;
    HAL_ADC_ConfigChannel(&hadc1, &sConfig);
    HAL_ADC_Start(&hadc1);
    HAL_ADC_PollForConversion(&hadc1, 10UL);
    measuredValues[numberOfSamples % 20] = HAL_ADC_GetValue(&hadc1);
    osMutexRelease(ADCmutexHandle); // End of the critical section
    numberOfSamples++;

    // Send message in mailqueue
    message = (data *)osMailAlloc(mailqueueHandle, 0);

    if (message != NULL)
    {
      message->source = 1;
      message->value = measuredValues[(numberOfSamples % 20) - 1];

      int condition = (numberOfSamples < 20 ? numberOfSamples : 20);

      // Calculate average value
      average = 0;
      for (int i = 0; i < condition; i++)
        average += measuredValues[i];
      average = average / condition;
      message->average = average;

      // Calculate variance
      variance = 0;
      for (int i = 0; i < condition; i++)
        variance += ((measuredValues[i] - average) * (measuredValues[i] - average));
      variance = variance / condition;
      message->variance = variance;

      message->timestamp = osKernelSysTick();
      osMailPut(mailqueueHandle, message);
    }

    endTime = osKernelSysTick();
    osDelay(30 - (endTime - startTime));
  }

  /* USER CODE END 5 */
}

/* StartTask2 function */
void StartTask2(void const *argument)
{
  /* USER CODE BEGIN StartTask2 */
  uint32_t startTime, endTime;
  static uint32_t numberOfSamples = 0;
  static float measuredValues[20];
  float average;
  float variance;
  data *message;
  /* Infinite loop */
  for (;;)
  {
    startTime = osKernelSysTick();

    osMutexWait(ADCmutexHandle, osWaitForever); // Start of the critical section -> use mutex
    sConfig.Channel = ADC_CHANNEL_1;
    HAL_ADC_ConfigChannel(&hadc1, &sConfig);
    HAL_ADC_Start(&hadc1);
    HAL_ADC_PollForConversion(&hadc1, 10UL);
    measuredValues[numberOfSamples % 20] = HAL_ADC_GetValue(&hadc1);
    osMutexRelease(ADCmutexHandle); // End of the critical section
    numberOfSamples++;

    // Send message in mailqueue
    message = (data *)osMailAlloc(mailqueueHandle, 0);

    if (message != NULL)
    {
      message->source = 2;
      message->value = measuredValues[(numberOfSamples % 20) - 1];

      int condition = (numberOfSamples < 20 ? numberOfSamples : 20);

      // calculate average value
      average = 0;
      for (int i = 0; i < condition; i++)
        average += measuredValues[i];
      average = average / condition;
      message->average = average;

      // calculate variance
      variance = 0;
      for (int i = 0; i < condition; i++)
        variance += ((measuredValues[i] - average) * (measuredValues[i] - average));
      variance = variance / condition;
      message->variance = variance;

      message->timestamp = osKernelSysTick();
      osMailPut(mailqueueHandle, message);
    }

    endTime = osKernelSysTick();
    osDelay(40 - (endTime - startTime));
  }

  /* USER CODE END StartTask2 */
}

/* StartTask3 function */
void StartTask3(void const *argument)
{
  /* USER CODE BEGIN StartTask3 */
  uint32_t startTime, endTime;
  static uint32_t numberOfSamples = 0;
  static float measuredValues[20];
  float average;
  float variance;
  data *message;
  /* Infinite loop */
  for (;;)
  {
    startTime = osKernelSysTick();

    osMutexWait(ADCmutexHandle, osWaitForever); // Start of the critical section -> use mutex
    sConfig.Channel = ADC_CHANNEL_2;
    HAL_ADC_ConfigChannel(&hadc1, &sConfig);
    HAL_ADC_Start(&hadc1);
    HAL_ADC_PollForConversion(&hadc1, 10UL);
    measuredValues[numberOfSamples % 20] = HAL_ADC_GetValue(&hadc1);
    osMutexRelease(ADCmutexHandle); // End of the critical section
    numberOfSamples++;

    // Send message in mailqueue
    message = (data *)osMailAlloc(mailqueueHandle, 0);

    if (message != NULL)
    {
      message->source = 3;
      message->value = measuredValues[(numberOfSamples % 20) - 1];

      int condition = (numberOfSamples < 20 ? numberOfSamples : 20);
      
      // Calculate average value
      average = 0;
      for (int i = 0; i < condition; i++)
        average += measuredValues[i];
      average = average / condition;
      message->average = average;

      // Calculate variance
      variance = 0;
      for (int i = 0; i < condition; i++)
        variance += ((measuredValues[i] - average) * (measuredValues[i] - average));
      variance = variance / condition;
      message->variance = variance;

      message->timestamp = osKernelSysTick();
      osMailPut(mailqueueHandle, message);
    }

    endTime = osKernelSysTick();
    osDelay(50 - (endTime - startTime));
  }

  /* USER CODE END StartTask3 */
}

/* StartTask4 function */
void StartTask4(void const *argument)
{
  /* USER CODE BEGIN StartTask4 */
  data *message;
  osEvent result;
  uint32_t source, timestamp;
  float value, average, variance;
  /* Infinite loop */
  for (;;)
  {
    result = osMailGet(mailqueueHandle, osWaitForever);
    message = result.value.p;

    source = message->source;
    value = message->value;
    average = message->average;
    variance = message->variance;
    timestamp = message->timestamp;

    osMailFree(mailqueueHandle, message);
    /* USER CODE END StartTask4 */
  }
}
/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM1 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM1)
  {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  file: The file name as string.
  * @param  line: The line in file as a number.
  * @retval None
  */
void _Error_Handler(char *file, int line)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
