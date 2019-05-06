/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"

I2C_HandleTypeDef hi2c1;
TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;
UART_HandleTypeDef huart1;

osThreadId main_isrHandle;
osThreadId setup_isrHandle;
osThreadId reserve_isrHandle;
/* Private function prototypes -----------------------------------------------*/
void HAL_I2C1_MspInit(I2C_HandleTypeDef* i2cHandle);
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_I2C1_Init(void);
static void MX_TIM2_Init(void);
static void MX_TIM3_Init(void);
void start_main_isr(void const * argument);
void start_setup_isr(void const * argument);
void start_reserve_isr(void const * argument);
void ScreenUpdate(void);
void PrintNumber(uint16_t number);
#define SS_DIGIT				4
// 7 segment font (0-9)
// D7=DP, D6=A, D5=B, D4=C, D3=D, D2=E, D1=F, D0=G
//const uint8_t font[10] ={0x7E, 0x30, 0x6D, 0x79, 0x33, 0x5B, 0x5F, 0x70, 0x7F, 0x7B};
const uint8_t font[96] = {	
	0x7E, /* 0 */
	0x30, /* 1 */
	0x6D, /* 2 */
	0x79, /* 3 */
	0x33, /* 4 */
	0x5B, /* 5 */
	0x5F, /* 6 */
	0x70, /* 7 */
	0x7F, /* 8 */
	0x7B, /* 9 */
	0x00, /* (space) */
	0x77, /* A */
	0x1F, /* B */
	0x4E, /* C */
	0x3D, /* D */
	0x4F, /* E */
	0x47, /* F */
	0x3D, /* G */
	0x76, /* H */
	0x30, /* I */
	0x1E, /* J */
	0x75, /* K */
	0x38, /* L */
	0x15, /* M */
	0x37, /* N */
	0x7E, /* O */
	0x67, /* P */
	0x6B, /* Q */
	0x33, /* R */
	0x5B, /* S */
	0x46, /* T */
	0x3E, /* U */
	0x3E, /* V */
	0x2A, /* W */
	0x76, /* X */
	0x6E, /* Y */
	0x5B, /* Z */
	0x09, /* : */
	0x0D, /* ; */
	0x61, /* < */
	0x48, /* = */
	0x43, /* > */
	0xD3, /* ? */
	0x5F, /* @ */	
	0x39, /* [ */
	0x64, /* \ */
	0x0F, /* ] */
	0x23, /* ^ */
	0x08, /* _ */
	0x02, /* ` */
	0x5F, /* a */
	0x7C, /* b */
	0x58, /* c */
	0x5E, /* d */
	0x7B, /* e */
	0x71, /* f */
	0x6F, /* g */
	0x74, /* h */
	0x10, /* i */
	0x0C, /* j */
	0x75, /* k */
	0x30, /* l */
	0x14, /* m */
	0x54, /* n */
	0x5C, /* o */
	0x73, /* p */
	0x67, /* q */
	0x50, /* r */
	0x6D, /* s */
	0x78, /* t */
	0x1C, /* u */
	0x1C, /* v */
	0x14, /* w */
	0x76, /* x */
	0x6E, /* y */
	0x5B, /* z */
	0x46, /* { */
	0x30, /* | */
	0x70, /* } */
	0x01, /* ~ */
	0x00, /* (del) */
	0x86, /* ! */
	0x22, /* " */
	0x7E, /* # */
	0x6D, /* $ */
	0xD2, /* % */
	0x46, /* & */
	0x20, /* ' */
	0x29, /* ( */
	0x0B, /* ) */
	0x21, /* * */
	0x70, /* + */
	0x10, /* , */
	0x40, /* - */
	0x80, /* . */
	0x52, /* / */
};
volatile uint8_t buffer[8]={29,30,25,26,10,10,10,10};
volatile uint8_t kiemtra_ctht=0, count_en=0;  
volatile uint32_t data_number=0, timer_counter=0, tg_hut=0, tg_nito=0, tg_han=0, tg_lammat=0, tg_xa=0;
//volatile int8_t BT_1=1, BT_2=1;
uint8_t data_load[15]={0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01};
uint8_t data_store_1[15]={0x00,0x06,0x03,0x00,0x05,0x06,0x00,0x08,0x02,0x00,0x04,0x05,0x01,0x07,0x08};
uint8_t data_store_2[15]={0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x02,0x03,0x04,0x05,0x06,0x07,0x08};
uint8_t data_store_3[15]={0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x02,0x03,0x04,0x05,0x06,0x07,0x08};
uint8_t data_store_4[15]={0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x02,0x03,0x04,0x05,0x06,0x07,0x08};
uint8_t data_store_5[15]={0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x02,0x03,0x04,0x05,0x06,0x07,0x08};
uint8_t data_store_6[15]={0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x02,0x03,0x04,0x05,0x06,0x07,0x08};
uint8_t data_store_7[15]={0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x02,0x03,0x04,0x05,0x06,0x07,0x08};
uint8_t data_store_8[15]={0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x02,0x03,0x04,0x05,0x06,0x07,0x08};
uint8_t data_store_9[15]={0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x02,0x03,0x04,0x05,0x06,0x07,0x08};
uint8_t data_store_10[15]={0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x02,0x03,0x04,0x05,0x06,0x07,0x08};
#define		_EEPROM_SIZE_KBIT										32
#define		_EEPROM24XX_I2C											hi2c1			
#define		_EEPROM_FREERTOS_IS_ENABLE					0
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
int	EEPROM24XX_IsConnected(void)
{
	if(HAL_I2C_IsDeviceReady(&_EEPROM24XX_I2C,0xa0,1,100)==HAL_OK)
		return 1;
	else
		return 0;	
}
//##########################################################################
int		EEPROM24XX_Save(uint16_t Address,void *data,size_t size_of_data)
{
	#if ((_EEPROM_SIZE_KBIT==1) || (_EEPROM_SIZE_KBIT==2))
	if(size_of_data > 8)
		return false;
	#endif
	#if ((_EEPROM_SIZE_KBIT==4) || (_EEPROM_SIZE_KBIT==8) || (_EEPROM_SIZE_KBIT==16))
	if(size_of_data > 16)
		return false;
	#endif
	#if ((_EEPROM_SIZE_KBIT==32) || (_EEPROM_SIZE_KBIT==64) || (_EEPROM_SIZE_KBIT==128))
	if(size_of_data > 32)
		return 0;
	#endif
	
	#if ((_EEPROM_SIZE_KBIT==1) || (_EEPROM_SIZE_KBIT==2))
	if(HAL_I2C_Mem_Write(&_EEPROM24XX_I2C,0xa0,Address,I2C_MEMADD_SIZE_8BIT,(uint8_t*)data,size_of_data,100) == HAL_OK)
	#else
	if(HAL_I2C_Mem_Write(&_EEPROM24XX_I2C,0xa0,Address,I2C_MEMADD_SIZE_16BIT,(uint8_t*)data,size_of_data,100) == HAL_OK)
	#endif
	{
		#if (_EEPROM_FREERTOS_IS_ENABLE==1)
		osDelay(7);
		#else
		HAL_Delay(7);
		#endif
		return 1;
	}
	else
		return 0;		
}
//##########################################################################
int EEPROM24XX_Load(uint16_t Address,void *data,size_t size_of_data)
{
	#if ((_EEPROM_SIZE_KBIT==1) || (_EEPROM_SIZE_KBIT==2))
	if(HAL_I2C_Mem_Read(&_EEPROM24XX_I2C,0xa0,Address,I2C_MEMADD_SIZE_8BIT,(uint8_t*)data,size_of_data,100) == HAL_OK)
	#else
	if(HAL_I2C_Mem_Read(&_EEPROM24XX_I2C,0xa0,Address,I2C_MEMADD_SIZE_16BIT,(uint8_t*)data,size_of_data,100) == HAL_OK)
	#endif
	{
		return 1;
	}
	else
		return 0;		
}
void HAL_I2C1_MspInit(I2C_HandleTypeDef* i2cHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct;
  if(i2cHandle->Instance==I2C1)
  {
	__HAL_RCC_I2C1_CLK_ENABLE();
  
    /**I2C1 GPIO Configuration    
    PB6     ------> I2C1_SCL
    PB7     ------> I2C1_SDA 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    /* Peripheral clock enable */
    __HAL_RCC_I2C1_CLK_ENABLE();
  }
  else if(i2cHandle->Instance==I2C2)
  {
	__HAL_RCC_I2C2_CLK_ENABLE();
    /**I2C2 GPIO Configuration    
    PB10     ------> I2C2_SCL
    PB11     ------> I2C2_SDA 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_10|GPIO_PIN_11;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    __HAL_RCC_I2C2_CLK_ENABLE();
  }
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
	MX_TIM3_Init();
	HAL_TIM_Base_Start_IT(&htim2);
	HAL_TIM_Base_Start_IT(&htim3);
	HAL_I2C1_MspInit(&hi2c1);
	while(EEPROM24XX_IsConnected()==0)
	{
		HAL_Delay(100);
	}
		EEPROM24XX_Save(0,data_store_1,15);	
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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
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
static void MX_I2C1_Init(void)
{
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 25000;
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
  htim2.Init.Period = 10;
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
static void MX_TIM3_Init(void)
{
  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  htim3.Instance = TIM3;
	//72MHz:36000_prescaler:Fcycle=2kHz:200~0.1s
  htim3.Init.Prescaler = 35999;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 200;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
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
	GPIO_InitTypeDef GPIO_InitStructA;
	GPIO_InitTypeDef GPIO_InitStructB;
	GPIO_InitTypeDef GPIO_InitStructC;
  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
	//Port A
	GPIO_InitStructA.Pin = 0x00ff;
  GPIO_InitStructA.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStructA.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStructA);
	//Port B
	GPIO_InitStructB.Pin = 0xffff;
  GPIO_InitStructB.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStructB.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStructB);
	//Port C
	GPIO_InitStructC.Pin = GPIO_PIN_0|GPIO_PIN_14;
  GPIO_InitStructC.Mode = GPIO_MODE_INPUT;
  GPIO_InitStructC.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStructC);
}
/* USER CODE END Header_start_main_isr */
void start_main_isr(void const * argument)
{
	printf("Device: drug product controller\r\n");
	printf("Revise: 28/04/2019\r\n");
	printf("Customer: Doan Van Luong@Domesco Dong Thap\r\n");
	printf("Designer: phongnh3289\r\n");
  for(;;)
  {
		printf("log command: Hello\r\n");
	/*	while(EEPROM24XX_IsConnected()==0)
	{
		HAL_Delay(100);
	}
		EEPROM24XX_Save(0,buff_a,8);	
		osDelay(1000);		
		EEPROM24XX_Load(0,data_load,15);	
		osDelay(1000);	
	*/
		osDelay(1000);			
  }
}

void start_setup_isr(void const * argument)
{
  for(;;)
  {
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
		osDelay(10);
		if(HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_0)==0){
			osDelay(10);
			while(HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_0)==0);
			data_number=data_number+1;
		}
		if(HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_14)==0){
			osDelay(10);
			while(HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_14)==0);
			data_number=data_number-1;
		}
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);			
		if(count_en==1)PrintNumber(data_number);
		osDelay(20);
  }
}
/* USER CODE END Header_start_reserve_isr */
void start_reserve_isr(void const * argument)
{
  for(;;)
  {
    if(kiemtra_ctht==1){
		while(EEPROM24XX_IsConnected()==0)HAL_Delay(100);	
		EEPROM24XX_Load(0,data_load,15);
		//Thoi gian setup
		tg_hut=data_load[0]*100+data_load[1]*10+data_load[2];
		tg_nito=data_load[3]*100+data_load[4]*10+data_load[5];
		tg_han=data_load[6]*100+data_load[7]*10+data_load[8];
		tg_lammat=data_load[9]*100+data_load[10]*10+data_load[11];
		tg_xa=data_load[12]*100+data_load[13]*10+data_load[14];
		//Chay hut chan khong
		timer_counter=0;
		data_number=0;
		count_en=1;
		buffer[0]=11;
		while(tg_hut!=timer_counter);
		
		//Chay hut nito
		timer_counter=0;
		data_number=0;
		buffer[0]=12;
		while(tg_nito!=timer_counter);	
			
		//Chay hut han
		timer_counter=0;
		data_number=0;
		buffer[0]=13;
		while(tg_han!=timer_counter);
		
		//Chay hut lam mat
		timer_counter=0;
		data_number=0;
		buffer[0]=14;
		while(tg_lammat!=timer_counter);
		
		//Chay hut xa
		timer_counter=0;
		data_number=0;
		buffer[0]=15;
		while(tg_xa!=timer_counter);
		kiemtra_ctht=0;
		count_en=0;
		buffer[0]=29;buffer[1]=30;buffer[2]=25;buffer[3]=26;buffer[4]=11;buffer[5]=11;buffer[6]=11;buffer[7]=11;
		}
		osDelay(1);  
}
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if (htim->Instance == TIM2) {
		ScreenUpdate();
  }
	if (htim->Instance == TIM1) {
    HAL_IncTick();
  }
	if (htim->Instance == TIM3) {
		if(timer_counter>1000)timer_counter=0;
		if(count_en==1)data_number=timer_counter;
		timer_counter++;
  }
}
void Error_Handler(void)
{
}
void PrintNumber(uint16_t number)
{
	// Check max and min
	if (number > 9999)number = 0;
	// Convert integer to bcd digits
	//buffer[0] = number / 1000;
	buffer[1] = number % 1000 / 100;
	if(buffer[1]==0)buffer[1]=10;
	buffer[2] = number % 100 / 10;
	buffer[3] = number % 10;
}
void ScreenUpdate(void)
{
	static uint8_t digit = 0;
	if(digit==0){
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_SET);	
	}
	else if(digit==1){HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_RESET);HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_SET);}
	else if(digit==2){HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_RESET);HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10, GPIO_PIN_SET);}
	else if(digit==3){HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10, GPIO_PIN_RESET);HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11, GPIO_PIN_SET);}
	if((digit==2)&&(count_en==0))GPIOA->ODR = (font[buffer[digit]]) & 0x00FF;
	else if((digit==2)&&(count_en==1))GPIOA->ODR = (font[buffer[digit]]|0x80) & 0x00FF;
	else GPIOA->ODR = (font[buffer[digit]]) & 0x00FF;	
	digit++;
	if (digit > (SS_DIGIT-1))digit = 0;
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
