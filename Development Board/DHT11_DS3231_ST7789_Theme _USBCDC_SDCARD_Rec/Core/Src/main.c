/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
#include "main.h"
#include "fatfs.h"
#include "usb_device.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "DHT.h"
#include "ds3231.h"
#include "st7789.h"
#include <string.h>
#define textColor LIGHTBLUE//0x2a1a//BLACK
#define bgColor BLACK//0xdefb
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
FATFS fs;
FATFS *pfs;
FIL fil;
DWORD fre_clust;
uint32_t totalSpace, freeSpace;
char buf[70];
uint8_t recStatus=0;
uint8_t recFlash=1;
uint8_t fileOpenFlag=0;
uint8_t fileCloseFlag=0;
struct tm timestamp;
time_t unixTime = 0;
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

SPI_HandleTypeDef hspi1;
SPI_HandleTypeDef hspi2;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_SPI1_Init(void);
static void MX_SPI2_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
time_t getUNIX(void)
{
	timestamp.tm_year = DS3231_GetYear();
	timestamp.tm_mon  = DS3231_GetMonth();
	timestamp.tm_mday = DS3231_GetDate();
	timestamp.tm_hour = DS3231_GetHour();
	timestamp.tm_min  = DS3231_GetMinute();
	timestamp.tm_sec  = DS3231_GetSecond();
	timestamp.tm_year -= 1900;
	timestamp.tm_mon -= 1;
	timestamp.tm_isdst = 0;
	unixTime = mktime(&timestamp);
	return unixTime;
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	static DHT_sensor livingRoom = {DHT11_GPIO_Port, DHT11_Pin, DHT11, GPIO_NOPULL};
	DS3231_Init(&hi2c1);
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
  MX_I2C1_Init();
  MX_SPI1_Init();
  MX_USB_DEVICE_Init();
  MX_SPI2_Init();
  MX_FATFS_Init();
  /* USER CODE BEGIN 2 */
	ST7789_Init();
	ST7789_Fill_Color(bgColor);
	ST7789_WriteString(10, 30, "MAX 65.3", Font_11x18, textColor, bgColor);
	ST7789_WriteString(10, 60, "MIN 51.4", Font_11x18, textColor, bgColor);
	ST7789_WriteString(203, 40, "o", Font_7x10, textColor, bgColor);
	ST7789_WriteString(210, 40, "C", Font_11x18, textColor, bgColor);
	ST7789_WriteString(210, 108, "%", Font_11x18, textColor, bgColor);
//	ST7789_WriteString(70, 108, "HUM", Font_11x18, textColor, bgColor);
	ST7789_WriteString(7, 190, "RUN TIME", Font_11x18, textColor, bgColor);
	ST7789_WriteString(100, 210, "DAYS  HR:MIN", Font_11x18, textColor, bgColor);
	
	uint8_t fresult;
	fresult = f_mount(&fs, "/", 1);
	sprintf(buf,(fresult != FR_OK) ? "ERROR!!! in mounting SD CARD...\n\n" : "SD CARD mounted successfully...\n\n");
	CDC_Transmit_FS((uint8_t*)buf,strlen(buf));
	
//	fresult = f_getfree("/", &fre_clust, &pfs);
//	printf("Error in checking size");
//	
//	totalSpace = (uint32_t)((pfs->n_fatent - 2) * pfs->csize * 0.5);
//	freeSpace = (uint32_t)(fre_clust * pfs->csize * 0.5);
	


	

	//ST7789_DrawFilledRectangle(0,0,100,100,RED);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	float maxTemp=0,minTemp=100;
	char unixTimeBuf[15];
  while (1)
  {
		
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
		//sprintf(buf,"%ld\n",(long)getUNIX());
		//CDC_Transmit_FS((uint8_t*)buf,strlen(buf));
		
			
		DHT_data d = DHT_getData(&livingRoom); 
		maxTemp = ( maxTemp < d.temp ) ?  d.temp : maxTemp;
		minTemp = ( minTemp < d.temp ) ?  minTemp  : d.temp;

		sprintf(buf, "%0.1f", d.temp);
		ST7789_WriteString(135, 40, buf, Font_16x26, textColor, bgColor);
		
		sprintf(buf, "%0.1f", d.hum);
		ST7789_WriteString(135, 108, buf, Font_16x26, textColor, bgColor);
		
		sprintf(buf, "MAX %0.1f", maxTemp);
		ST7789_WriteString(10, 30, buf, Font_11x18, textColor, bgColor);
		sprintf(buf, "MIN %0.1f", minTemp);
		ST7789_WriteString(10, 60, buf, Font_11x18, textColor, bgColor);
		
		sprintf(buf ,"%02d %02d:%02d" ,DS3231_GetDate(),DS3231_GetHour(),DS3231_GetMinute());
		ST7789_WriteString(105, 175, buf, Font_16x26, textColor, bgColor);
		//ST7789_WriteString(65, 150, buf, Font_11x18, textColor, bgColor);
		//sprintf(buf, "%d : %d : %d",DS3231_GetHour(),DS3231_GetMinute(),DS3231_GetSecond());
		//ST7789_WriteString(55, 180, buf, Font_11x18, textColor, bgColor);
		
		if (recStatus) {
			if (recFlash) {
				ST7789_WriteString(50, 108, "REC", Font_11x18, textColor, bgColor);
				ST7789_WriteString(90, 106, "o", Font_11x18, textColor, bgColor);
			}
			else 
			{
				ST7789_WriteString(50, 108, "      ", Font_11x18, textColor, bgColor);
			}
			recFlash= !recFlash;
			
			if(fileOpenFlag==1) {
				sprintf(unixTimeBuf,"%ld",(long)unixTime);
				sprintf(&unixTimeBuf[8],".txt",(long)unixTime);
				fresult = f_open(&fil, unixTimeBuf, FA_OPEN_APPEND | FA_WRITE | FA_READ);
				sprintf(buf,(fresult != FR_OK) ? "Error in Opening File\n" :"File opened succesfully\n");
				CDC_Transmit_FS((uint8_t*)buf,strlen(buf));
				fileOpenFlag=2;
				fileCloseFlag =1;
			}
				
			sprintf(buf ,"%02d/%02d/%02d -> %02d:%02d:%02d\nTemp : %0.2f\nHum : %0.2f\n" ,DS3231_GetDate(),DS3231_GetMonth(),DS3231_GetYear(),DS3231_GetHour(),DS3231_GetMinute(),DS3231_GetSecond(),d.temp,d.hum);	
			fresult = f_puts(buf, &fil);
			sprintf(buf,(fresult < 0) ? "Unable to write on file\n" : "Data written in file\n");
			CDC_Transmit_FS((uint8_t*)buf,strlen(buf));
			
			
		}
		else
		{
			ST7789_WriteString(50, 108, "      ", Font_11x18, textColor, bgColor);
			if(fileCloseFlag==1) {
				fresult = f_close(&fil);
				sprintf(buf,(fresult != FR_OK) ? "Error in closing file\n" : "File closed succesfully\n");
				CDC_Transmit_FS((uint8_t*)buf,strlen(buf));
				fileCloseFlag=0;
			}
		}
		
		
		HAL_Delay(1000);
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
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief SPI2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI2_Init(void)
{

  /* USER CODE BEGIN SPI2_Init 0 */

  /* USER CODE END SPI2_Init 0 */

  /* USER CODE BEGIN SPI2_Init 1 */

  /* USER CODE END SPI2_Init 1 */
  /* SPI2 parameter configuration*/
  hspi2.Instance = SPI2;
  hspi2.Init.Mode = SPI_MODE_MASTER;
  hspi2.Init.Direction = SPI_DIRECTION_2LINES;
  hspi2.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi2.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi2.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi2.Init.NSS = SPI_NSS_SOFT;
  hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;
  hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi2.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI2_Init 2 */

  /* USER CODE END SPI2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(ST7789_CS_GPIO_Port, ST7789_CS_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, ST7789_RST_Pin|ST7789_DC_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(SD_CS_GPIO_Port, SD_CS_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin : REC_Pin */
  GPIO_InitStruct.Pin = REC_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(REC_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : ST7789_CS_Pin */
  GPIO_InitStruct.Pin = ST7789_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(ST7789_CS_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : ST7789_RST_Pin ST7789_DC_Pin */
  GPIO_InitStruct.Pin = ST7789_RST_Pin|ST7789_DC_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : SD_CS_Pin */
  GPIO_InitStruct.Pin = SD_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(SD_CS_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : DHT11_Pin */
  GPIO_InitStruct.Pin = DHT11_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(DHT11_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI0_IRQn);

}

/* USER CODE BEGIN 4 */

int fputc(int ch, FILE *f)
{
  ITM_SendChar(ch);
  return(ch);
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
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
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
