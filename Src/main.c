/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"

I2C_HandleTypeDef hi2c1;
TIM_HandleTypeDef htim2;
UART_HandleTypeDef huart1;

osThreadId main_isrHandle;
osThreadId setup_isrHandle;
osThreadId reserve_isrHandle;
/* Private function prototypes -----------------------------------------------*/
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_I2C1_Init(void);
static void MX_TIM2_Init(void);
void start_main_isr(void const * argument);
void start_setup_isr(void const * argument);
void start_reserve_isr(void const * argument);
void ScreenUpdate(void);

#define SS_DIGIT				4
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
  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART1_UART_Init();
  MX_I2C1_Init();
  MX_TIM2_Init();
	HAL_TIM_Base_Start_IT(&htim2);
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

void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
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

static void MX_TIM2_Init(void)
{
  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 35999;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 800;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
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
static void MX_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
	GPIO_InitStruct.Pin = 0xffff;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

void start_main_isr(void const * argument)
{
	printf("Device: drug product controller\r\n");
	printf("Revise: 28/04/2019\r\n");
	printf("Customer: Doan Van Luong@Domesco Dong Thap\r\n");
	printf("Designer: phongnh3289\r\n");
  for(;;)
  {
    printf("log command: Hello\r\n");
		 osDelay(1000);
  }
}
void start_setup_isr(void const * argument)
{
  for(;;)
  {
    osDelay(1);
  }
}
void start_reserve_isr(void const * argument)
{
  for(;;)
  {
    //HAL_Delay(2);
		//ScreenUpdate();
		osDelay(1);
  }
}
void ScreenUpdate(void)
{
	static uint8_t digit = 0;
	if(digit==0){HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11, GPIO_PIN_RESET);HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_SET);}
	else if(digit==1){HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_RESET);HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_SET);}
	else if(digit==2){HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_RESET);HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10, GPIO_PIN_SET);}
	else if(digit==3){HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10, GPIO_PIN_RESET);HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11, GPIO_PIN_SET);}
	//else if(digit==4){HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11, GPIO_PIN_RESET);HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);}
	//else if(digit==5){HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_SET);}
	//else if(digit==6){HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_RESET);HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET);}
	//else if(digit==7){HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, GPIO_PIN_SET);}
	GPIOA->ODR = (~font[buffer[digit]]) & 0x00FF;	
	digit++;
	if (digit > (SS_DIGIT-1))digit = 0;
}
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if (htim->Instance == TIM2) {
    //HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0);
		ScreenUpdate();
  }
  if (htim->Instance == TIM1) {
    HAL_IncTick();
  }
}
void Error_Handler(void)
{

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
