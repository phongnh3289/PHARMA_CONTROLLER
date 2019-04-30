/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"
#include "stm32f1xx_hal_i2c.h"

I2C_HandleTypeDef hi2c1;
UART_HandleTypeDef huart1;

osThreadId main_isrHandle;
osThreadId setup_isrHandle;
osThreadId reserve_isrHandle;
void ScreenUpdate(void);
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_I2C1_Init(void);
void start_main_isr(void const * argument);
void start_setup_isr(void const * argument);
void start_reserve_isr(void const * argument);
#define SS_DIGIT				8
const uint8_t font[10] ={0x7E, 0x30, 0x6D, 0x79, 0x33, 0x5B, 0x5F, 0x70, 0x7F, 0x7B};
uint8_t buffer[8]={1,2,3,4,5,6,7,8};
/* Private function prototypes -----------------------------------------------*/
#ifdef __GNUC__
  /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
  #define GETCHAR_PROTOTYPE int fgetc(FILE *f)
#endif /* __GNUC__ */
	PUTCHAR_PROTOTYPE
{
	HAL_UART_Transmit(&huart1, (uint8_t*)&ch,1,100);
  return ch;
}
int main(void)
{
  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();
  MX_USART1_UART_Init();
  MX_I2C1_Init();
  /* Create the thread(s) */
  /* definition and creation of main_isr */
  osThreadDef(main_isr, start_main_isr, osPriorityNormal, 0, 128);
  main_isrHandle = osThreadCreate(osThread(main_isr), NULL);

  /* definition and creation of setup_isr */
  osThreadDef(setup_isr, start_setup_isr, osPriorityIdle, 0, 128);
  setup_isrHandle = osThreadCreate(osThread(setup_isr), NULL);

  /* definition and creation of reserve_isr */
  osThreadDef(reserve_isr, start_reserve_isr, osPriorityIdle, 0, 128);
  reserve_isrHandle = osThreadCreate(osThread(reserve_isr), NULL);
  /* Start scheduler */

  osKernelStart();
  while (1);
}

void start_main_isr(void const * argument)
{
  printf("Device: drug product controller\r\n");
	printf("Revise: 28/04/2019\r\n");
	printf("Customer: Doan Van Luong@Domesco Dong Thap\r\n");
	printf("Designer: phongnh3289\r\n");
	for(;;)
  {
    osDelay(1);
		printf("log command: Hello\r\n");
		HAL_Delay(500);
  }
  /* USER CODE END 5 */ 
}

void start_setup_isr(void const * argument)
{
  for(;;)
  {
    osDelay(1);
		ScreenUpdate();
		HAL_Delay(10000); // Delay
  }
  /* USER CODE END start_setup_isr */
}

void start_reserve_isr(void const * argument)
{

  for(;;)
  {
    osDelay(1);
  }

}
void ScreenUpdate(void)
{
	static uint8_t digit = 0;
	if(digit==0){HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, GPIO_PIN_RESET);HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_SET);}
	else if(digit==1){HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_RESET);HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_SET);}
	else if(digit==2){HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_RESET);HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10, GPIO_PIN_SET);}
	else if(digit==3){HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10, GPIO_PIN_RESET);HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11, GPIO_PIN_SET);}
	else if(digit==4){HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11, GPIO_PIN_RESET);HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);}
	else if(digit==5){HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_SET);}
	else if(digit==6){HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_RESET);HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET);}
	else if(digit==7){HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, GPIO_PIN_SET);}
	GPIOA->ODR |= (~font[buffer[digit]]) & 0x00FF;	
	digit++;
	if (digit > (SS_DIGIT-1))digit = 0;
}
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  /**Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI_DIV2;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL8;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /**Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
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

}

static void MX_USART1_UART_Init(void)
{
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
/*Configure GPIO pin Output Level */

  GPIO_InitStruct.Pin = 0xffff;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if (htim->Instance == TIM1) {
    HAL_IncTick();
  }
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

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
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
