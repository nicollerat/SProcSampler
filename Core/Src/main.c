/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#include "main.h"
#include "adc.h"
#include "dac.h"
#include "dma.h"
#include "i2c.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "string.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "app.h"
#include "stdio.h"
#include "stdbool.h"

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

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

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
  MX_DMA_Init();
  MX_USART2_UART_Init();
  MX_ADC1_Init();
  MX_DAC_Init();
  MX_TIM1_Init();
  MX_ADC2_Init();
  MX_I2C1_Init();
  MX_SPI2_Init();
  MX_SPI3_Init();
  MX_USART6_UART_Init();
  MX_TIM10_Init();
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */

  HAL_GPIO_WritePin(RE_LED1_GPIO_Port, RE_LED1_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(RE_LED2_GPIO_Port, RE_LED2_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(GR_LED3_GPIO_Port, GR_LED3_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(GR_LED4_GPIO_Port, GR_LED4_Pin, GPIO_PIN_SET);

  HAL_DAC_Start(&hdac, DAC_CHANNEL_1);
  HAL_DAC_Start(&hdac, DAC_CHANNEL_2);

  HAL_GPIO_WritePin(RE_LED1_GPIO_Port, RE_LED1_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(RE_LED2_GPIO_Port, RE_LED2_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GR_LED3_GPIO_Port, GR_LED3_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GR_LED4_GPIO_Port, GR_LED4_Pin, GPIO_PIN_RESET);

  HAL_TIM_Base_Start(&htim1);
  HAL_TIM_Base_Start(&htim2); // Trigger of the ADCs
  HAL_TIM_Base_Start_IT(&htim10);

  HAL_ADC_Start_IT(&hadc1);
  HAL_ADC_Start(&hadc2);

  app_init();
  initUart();
  sendUart("Starting !\n");

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  app_loop();
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
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
	uint16_t v1=HAL_ADC_GetValue(&hadc1);
	uint16_t v2=HAL_ADC_GetValue(&hadc2);
	app_adc(v1,v2);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	// Call timer function in the application
	app_timer(htim);
}

int _write(int file, char *ptr, int len)
{
    HAL_UART_Transmit(&huart2, (uint8_t *)ptr, len, HAL_MAX_DELAY);
    return len;
}

void sendUart(char * str)
{
	_write(0, str, strlen(str));
}

// Uses 2 buffers to construct received strings
char recBuffer[2][128];
int recIndex=0;
int nbRecIn=0;
int currentBuffer=0;
#define EOL 13
bool newLine=false;
int uartOverflow=0;
int lineOverflow=0;
char * pRec=0;

void initUart()
{
	HAL_UART_Receive_IT(&huart2, (uint8_t*)&recBuffer[0][0], 1);
}

// Overload the weak function of the library
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if (recBuffer[currentBuffer][recIndex]==EOL) {
		if (!newLine) {
			newLine=true;
			nbRecIn=recIndex;
			pRec=recBuffer[currentBuffer];
		} else {
			lineOverflow++;
		}
		currentBuffer=1-currentBuffer;
		recIndex=0;
	} else {
		recIndex++;
		if (recIndex >= sizeof(recBuffer[0])) {
			uartOverflow++;
			recIndex=0;
		}
	}

	HAL_UART_Receive_IT(&huart2, (uint8_t*)&recBuffer[currentBuffer][recIndex], 1);
}

// Read UART buffer and return the number of chars
int getUart(char * str)
{
	if (newLine) {
		newLine=false;
		memcpy(str, pRec, nbRecIn);
		str[nbRecIn]=0;
		return nbRecIn;
	}
	str[0]=0;
	return 0;
}

// Change the PWM frequency. TIM1 generates the PWM and TIM2 triggers the ADC
void setPWMFreq(float freq)
{
	uint32_t Period = HAL_RCC_GetPCLK1Freq()*2 / freq;
	htim1.Instance->ARR = Period ;
	htim2.Instance->ARR = Period ;
}

// Enable the PWM outputs
void enablePWM(bool en)
{
	if (en) {
		HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
		HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
		HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);
	} else {
		HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
		HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_2);
		HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_3);
	}
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  printf("Stuck in Error_Handler\n\r");
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
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
