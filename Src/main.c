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
void update_data(uint8_t mode);
static void MX_USART1_UART_Init(void);
static void MX_I2C1_Init(void);
static void MX_TIM2_Init(void);
static void MX_TIM3_Init(void);
void start_main_isr(void const * argument);
void start_setup_isr(void const * argument);
void start_reserve_isr(void const * argument);
void ScreenUpdate(void);
void PrintNumber(uint16_t number);
#define SS_DIGIT				8
// 7 segment font (0-9)
// D7=DP, D6=A, D5=B, D4=C, D3=D, D2=E, D1=F, D0=G
//const uint8_t font[10] ={0x7E, 0x30, 0x6D, 0x79, 0x33, 0x5B, 0x5F, 0x70, 0x7F, 0x7B};
/* Rows */
/* Row 1 default */
#ifndef KEYPAD_ROW_1_PIN
#define KEYPAD_ROW_1_PORT			GPIOB
#define KEYPAD_ROW_1_PIN			GPIO_PIN_0
#endif
/* Row 2 default */
#ifndef KEYPAD_ROW_2_PIN
#define KEYPAD_ROW_2_PORT			GPIOB
#define KEYPAD_ROW_2_PIN			GPIO_PIN_1
#endif
/* Columns */
/* Column 1 default */
#ifndef KEYPAD_COLUMN_1_PIN
#define KEYPAD_COLUMN_1_PORT		GPIOC
#define KEYPAD_COLUMN_1_PIN			GPIO_PIN_8
#endif
/* Column 2 default */
#ifndef KEYPAD_COLUMN_2_PIN
#define KEYPAD_COLUMN_2_PORT		GPIOC
#define KEYPAD_COLUMN_2_PIN			GPIO_PIN_9
#endif
/* Column 3 default */
#ifndef KEYPAD_COLUMN_3_PIN
#define KEYPAD_COLUMN_3_PORT		GPIOC
#define KEYPAD_COLUMN_3_PIN			GPIO_PIN_10
#endif
/* Column 4 default */
#ifndef KEYPAD_COLUMN_4_PIN
#define KEYPAD_COLUMN_4_PORT		GPIOC
#define KEYPAD_COLUMN_4_PIN			GPIO_PIN_11
#endif
/* Column 5 default */
#ifndef KEYPAD_COLUMN_5_PIN
#define KEYPAD_COLUMN_5_PORT		GPIOC
#define KEYPAD_COLUMN_5_PIN			GPIO_PIN_12
#endif
/* Column 6 default */
#ifndef KEYPAD_COLUMN_6_PIN
#define KEYPAD_COLUMN_6_PORT		GPIOC
#define KEYPAD_COLUMN_6_PIN			GPIO_PIN_13
#endif
/* Column 7 default */
#ifndef KEYPAD_COLUMN_7_PIN
#define KEYPAD_COLUMN_7_PORT		GPIOC
#define KEYPAD_COLUMN_7_PIN			GPIO_PIN_14
#endif
/* Number of milliseconds between 2 reads */
#ifndef KEYPAD_READ_INTERVAL
#define KEYPAD_READ_INTERVAL        10
#endif

/* Keypad no pressed */
#define KEYPAD_NO_PRESSED			(uint8_t)0xFF
typedef enum {
	TM_KEYPAD_Button_0 = 0x00,                     /*!< Button 0 code */
	TM_KEYPAD_Button_1 = 0x01,                     /*!< Button 1 code */
	TM_KEYPAD_Button_2 = 0x02,                     /*!< Button 2 code */
	TM_KEYPAD_Button_3 = 0x03,                     /*!< Button 3 code */
	TM_KEYPAD_Button_4 = 0x04,                     /*!< Button 4 code */
	TM_KEYPAD_Button_5 = 0x05,                     /*!< Button 5 code */
	TM_KEYPAD_Button_6 = 0x06,                     /*!< Button 6 code */
	TM_KEYPAD_Button_7 = 0x07,                     /*!< Button 7 code */
	TM_KEYPAD_Button_8 = 0x08,                     /*!< Button 8 code */
	TM_KEYPAD_Button_9 = 0x09,                     /*!< Button 9 code */
	TM_KEYPAD_Button_STAR = 0x0A,                  /*!< Button START code */
	TM_KEYPAD_Button_HASH = 0x0B,                  /*!< Button HASH code */
	TM_KEYPAD_Button_A = 0x0C,	                   /*!< Button A code. Only on large size */
	TM_KEYPAD_Button_B = 0x0D,	                   /*!< Button B code. Only on large size */
	TM_KEYPAD_Button_C = 0x0E,	                   /*!< Button C code. Only on large size */
	TM_KEYPAD_Button_D = 0x0F,	                   /*!< Button D code. Only on large size */
	TM_KEYPAD_Button_NOPRESSED = KEYPAD_NO_PRESSED /*!< No button pressed */
} TM_KEYPAD_Button_t;
typedef enum {
	TM_KEYPAD_Type_Large = 0x00, /*!< Keypad 4x4 size */
	TM_KEYPAD_Type_Small         /*!< Keypad 3x4 size */
} TM_KEYPAD_Type_t;
void TM_KEYPAD_Init(TM_KEYPAD_Type_t type);
TM_KEYPAD_Button_t TM_KEYPAD_Read(void);
void TM_KEYPAD_Update(void);
#define KEYPAD_COLUMN_1_HIGH		HAL_GPIO_WritePin(KEYPAD_COLUMN_1_PORT, KEYPAD_COLUMN_1_PIN,GPIO_PIN_SET)
#define KEYPAD_COLUMN_1_LOW			HAL_GPIO_WritePin(KEYPAD_COLUMN_1_PORT, KEYPAD_COLUMN_1_PIN,GPIO_PIN_RESET)
#define KEYPAD_COLUMN_2_HIGH		HAL_GPIO_WritePin(KEYPAD_COLUMN_2_PORT, KEYPAD_COLUMN_2_PIN,GPIO_PIN_SET)
#define KEYPAD_COLUMN_2_LOW			HAL_GPIO_WritePin(KEYPAD_COLUMN_2_PORT, KEYPAD_COLUMN_2_PIN,GPIO_PIN_RESET)
#define KEYPAD_COLUMN_3_HIGH		HAL_GPIO_WritePin(KEYPAD_COLUMN_3_PORT, KEYPAD_COLUMN_3_PIN,GPIO_PIN_SET)
#define KEYPAD_COLUMN_3_LOW			HAL_GPIO_WritePin(KEYPAD_COLUMN_3_PORT, KEYPAD_COLUMN_3_PIN,GPIO_PIN_RESET)
#define KEYPAD_COLUMN_4_HIGH		HAL_GPIO_WritePin(KEYPAD_COLUMN_4_PORT, KEYPAD_COLUMN_4_PIN,GPIO_PIN_SET)
#define KEYPAD_COLUMN_4_LOW			HAL_GPIO_WritePin(KEYPAD_COLUMN_4_PORT, KEYPAD_COLUMN_4_PIN,GPIO_PIN_RESET)
#define KEYPAD_COLUMN_5_HIGH		HAL_GPIO_WritePin(KEYPAD_COLUMN_5_PORT, KEYPAD_COLUMN_5_PIN,GPIO_PIN_SET)
#define KEYPAD_COLUMN_5_LOW			HAL_GPIO_WritePin(KEYPAD_COLUMN_5_PORT, KEYPAD_COLUMN_5_PIN,GPIO_PIN_RESET)
#define KEYPAD_COLUMN_6_HIGH		HAL_GPIO_WritePin(KEYPAD_COLUMN_6_PORT, KEYPAD_COLUMN_6_PIN,GPIO_PIN_SET)
#define KEYPAD_COLUMN_6_LOW			HAL_GPIO_WritePin(KEYPAD_COLUMN_6_PORT, KEYPAD_COLUMN_6_PIN,GPIO_PIN_RESET)
#define KEYPAD_COLUMN_7_HIGH		HAL_GPIO_WritePin(KEYPAD_COLUMN_7_PORT, KEYPAD_COLUMN_7_PIN,GPIO_PIN_SET)
#define KEYPAD_COLUMN_7_LOW			HAL_GPIO_WritePin(KEYPAD_COLUMN_7_PORT, KEYPAD_COLUMN_7_PIN,GPIO_PIN_RESET)

/* Read input pins */
#define KEYPAD_ROW_1_CHECK			(!HAL_GPIO_ReadPin(KEYPAD_ROW_1_PORT, KEYPAD_ROW_1_PIN))
#define KEYPAD_ROW_2_CHECK			(!HAL_GPIO_ReadPin(KEYPAD_ROW_2_PORT, KEYPAD_ROW_2_PIN))


uint8_t KEYPAD_INT_Buttons[2][8] = {
	{0x02, 0x03, 0x05, 'i', 0x0a, 'o', 's', 0x07},
	{0x01, 0x04, 0x0b, 'p', 0x0d, 'c', 'd', 0x08},
};

/* Private functions */
void TM_KEYPAD_INT_SetColumn(uint8_t column);
uint8_t TM_KEYPAD_INT_CheckRow(uint8_t column);
uint8_t TM_KEYPAD_INT_Read(void);

/* Private variables */
TM_KEYPAD_Type_t TM_KEYPAD_INT_KeypadType;
static TM_KEYPAD_Button_t KeypadStatus = TM_KEYPAD_Button_NOPRESSED;

/* Private */
void TM_KEYPAD_INT_SetColumn(uint8_t column) {
	/* Set rows high */
	KEYPAD_COLUMN_1_HIGH;
	KEYPAD_COLUMN_2_HIGH;
	KEYPAD_COLUMN_3_HIGH;
	KEYPAD_COLUMN_4_HIGH;
	KEYPAD_COLUMN_5_HIGH;
	KEYPAD_COLUMN_6_HIGH;
	KEYPAD_COLUMN_7_HIGH;
	/* Set column low */
	if (column == 1) {
		KEYPAD_COLUMN_1_LOW;
	}
	if (column == 2) {
		KEYPAD_COLUMN_2_LOW;
	}
	if (column == 3) {
		KEYPAD_COLUMN_3_LOW;
	}
	if (column == 4) {
		KEYPAD_COLUMN_4_LOW;
	}
		if (column == 5) {
		KEYPAD_COLUMN_5_LOW;
	}
	if (column == 6) {
		KEYPAD_COLUMN_6_LOW;
	}
	if (column == 7) {
		KEYPAD_COLUMN_7_LOW;
	}
}

uint8_t TM_KEYPAD_INT_CheckRow(uint8_t column) {
	/* Read rows */
	
	/* Scan row 1 */
	if (KEYPAD_ROW_1_CHECK) {
		return KEYPAD_INT_Buttons[0][column - 1];	
	}
	/* Scan row 2 */
	if (KEYPAD_ROW_2_CHECK) {
		return KEYPAD_INT_Buttons[1][column - 1];
	}
	/* Not pressed */
	return KEYPAD_NO_PRESSED;
}

uint8_t TM_KEYPAD_INT_Read(void) {
	uint8_t check;
	/* Set row 1 to LOW */
	TM_KEYPAD_INT_SetColumn(1);
	/* Check rows */
	check = TM_KEYPAD_INT_CheckRow(1);
	if (check != KEYPAD_NO_PRESSED) {
		return check;
	}
	
	/* Set row 2 to LOW */
	TM_KEYPAD_INT_SetColumn(2);
	/* Check columns */
	check = TM_KEYPAD_INT_CheckRow(2);
	if (check != KEYPAD_NO_PRESSED) {
		return check;
	}
/* Set row 3 to LOW */
	TM_KEYPAD_INT_SetColumn(3);
	/* Check columns */
	check = TM_KEYPAD_INT_CheckRow(3);
	if (check != KEYPAD_NO_PRESSED) {
		return check;
	}
	/* Set row 4 to LOW */
	TM_KEYPAD_INT_SetColumn(4);
	/* Check columns */
	check = TM_KEYPAD_INT_CheckRow(4);
	if (check != KEYPAD_NO_PRESSED) {
		return check;
	}
	/* Set row 5 to LOW */
	TM_KEYPAD_INT_SetColumn(5);
	/* Check columns */
	check = TM_KEYPAD_INT_CheckRow(5);
	if (check != KEYPAD_NO_PRESSED) {
		return check;
	}
	/* Set row 6 to LOW */
	TM_KEYPAD_INT_SetColumn(6);
	/* Check columns */
	check = TM_KEYPAD_INT_CheckRow(6);
	if (check != KEYPAD_NO_PRESSED) {
		return check;
	}
	/* Set row 7 to LOW */
	TM_KEYPAD_INT_SetColumn(7);
	/* Check columns */
	check = TM_KEYPAD_INT_CheckRow(7);
	if (check != KEYPAD_NO_PRESSED) {
		return check;
	}
	else return KEYPAD_NO_PRESSED;
}

void TM_KEYPAD_Update(void) {
	static uint16_t millis = 0;
	
	/* Every X ms read */
	if (++millis >= KEYPAD_READ_INTERVAL && KeypadStatus == TM_KEYPAD_Button_NOPRESSED) {
		/* Reset */
		millis = 0;
		
		/* Read keyboard */
		KeypadStatus = (TM_KEYPAD_Button_t) TM_KEYPAD_INT_Read();
	}
}
const uint8_t font[96] = {	
	0x3f, /* 0 */
	0x06, /* 1 */
	0x5B, /* 2 */
	0x4F, /* 3 */
	0x66, /* 4 */
	0x6d, /* 5 */
	0x7d, /* 6 */
	0x07, /* 7 */
	0x7F, /* 8 */
	0x6f, /* 9 */
	0x00, /* (space) */
	0x77, /* A */
	0x7c, /* B */
	0x39, /* C */
	0x5e, /* D */
	0x79, /* E */
	0x71, /* F */
	0x3D, /* G */
	0x76, /* H */
	0x30, /* I */
	0x1E, /* J */
	0x75, /* K */
	0x38, /* L */
	0x15, /* M */
	0x37, /* N */
	0x5c, /* O */
	0x73, /* P */
	0x6B, /* Q */
	0x33, /* R */
	0x6D, /* S */
	0x31, /* T */
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
volatile uint8_t buffer[8]={29,30,25,26,10,10,10,10}, digit = 0, key_pad=0, dot_en=0;
volatile uint8_t kiemtra_ctht=0, count_en=0, program_number=1, program_en=0, mode_hut=0, mode_nito=0, mode_han=0, mode_lammat=0, mode_xa=0;  
volatile uint16_t data_number=0, timer_counter=0, tg_hut=0, tg_nito=0, tg_han=0, tg_lammat=0, tg_xa=0;

//volatile int8_t BT_1=1, BT_2=1;
uint8_t data_load[15]={0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01};
uint8_t data_store_1[15]={0x00,0x06,0x03,0x00,0x05,0x06,0x00,0x08,0x02,0x00,0x04,0x05,0x01,0x04,0x08};
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
	while(EEPROM24XX_IsConnected()==0)HAL_Delay(100);
	EEPROM24XX_Load(0,data_store_1,15);HAL_Delay(100);	
	EEPROM24XX_Load(0x10,data_store_2,15);HAL_Delay(100);
	EEPROM24XX_Load(0x20,data_store_3,15);HAL_Delay(100);	
	EEPROM24XX_Load(0x30,data_store_4,15);HAL_Delay(100);	
	EEPROM24XX_Load(0x40,data_store_5,15);HAL_Delay(100);	
	EEPROM24XX_Load(0x50,data_store_6,15);HAL_Delay(100);	
	EEPROM24XX_Load(0x60,data_store_7,15);HAL_Delay(100);	
	EEPROM24XX_Load(0x70,data_store_8,15);HAL_Delay(100);	
	EEPROM24XX_Load(0x80,data_store_9,15);HAL_Delay(100);	
	EEPROM24XX_Load(0x90,data_store_10,15);HAL_Delay(100);	
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
/*
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSEState = RCC_HSI_ON;
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
*/

void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI_DIV2;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL16;
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
  htim2.Init.Prescaler = 31999;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 5;
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
  htim3.Init.Prescaler = 31999;
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
  GPIO_InitStructA.Speed = GPIO_SPEED_FREQ_MEDIUM;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStructA);
	//Port B
	//GPIO_InitStructB.Pin = 0x00ff;
  //GPIO_InitStructB.Mode = GPIO_MODE_OUTPUT_PP;
  //GPIO_InitStructB.Speed = GPIO_SPEED_FREQ_HIGH;
  //HAL_GPIO_Init(GPIOB, &GPIO_InitStructB);
	GPIO_InitStructB.Pin = 0x00ff;
  GPIO_InitStructB.Mode = GPIO_MODE_INPUT;
  GPIO_InitStructB.Speed = GPIO_SPEED_FREQ_MEDIUM;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStructB);
	//Port C
	GPIO_InitStructC.Pin = 0xffff;
  GPIO_InitStructC.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStructC.Speed = GPIO_SPEED_FREQ_MEDIUM;
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
		if(count_en==1)PrintNumber(data_number);
		osDelay(10);			
  }
}
	
void start_setup_isr(void const * argument)
{
  for(;;)
  {
		KeypadStatus = (TM_KEYPAD_Button_t) TM_KEYPAD_INT_Read();
		if(KeypadStatus!=0xff)key_pad=KeypadStatus;
		if(key_pad==0x0a){kiemtra_ctht=1, key_pad=0;}
		if(kiemtra_ctht==0){
//mode hut chan khong		
		if(key_pad==0x01){
			key_pad=0;
			mode_hut=1;mode_han=0;mode_lammat=0;mode_nito=0;mode_xa=0;
			if(program_en==1){
				buffer[0]=11;buffer[1]=10;buffer[2]=10;buffer[3]=10;buffer[4]=10;
				if(program_number==1){buffer[5]=data_store_1[0];buffer[6]=data_store_1[1];buffer[7]=data_store_1[2];}
				else if(program_number==2){buffer[5]=data_store_2[0];buffer[6]=data_store_2[1];buffer[7]=data_store_2[2];}
				else if(program_number==3){buffer[5]=data_store_3[0];buffer[6]=data_store_3[1];buffer[7]=data_store_3[2];}
				else if(program_number==4){buffer[5]=data_store_4[0];buffer[6]=data_store_4[1];buffer[7]=data_store_4[2];}
				else if(program_number==5){buffer[5]=data_store_5[0];buffer[6]=data_store_5[1];buffer[7]=data_store_5[2];}
				else if(program_number==6){buffer[5]=data_store_6[0];buffer[6]=data_store_6[1];buffer[7]=data_store_6[2];}
				else if(program_number==7){buffer[5]=data_store_7[0];buffer[6]=data_store_7[1];buffer[7]=data_store_7[2];}
				else if(program_number==8){buffer[5]=data_store_8[0];buffer[6]=data_store_8[1];buffer[7]=data_store_8[2];}
				else if(program_number==9){buffer[5]=data_store_9[0];buffer[6]=data_store_9[1];buffer[7]=data_store_9[2];}
				else if(program_number==10){buffer[5]=data_store_10[0];buffer[6]=data_store_10[1];buffer[7]=data_store_10[2];}	
				data_number=100*buffer[5]+10*buffer[6]+buffer[7];
				dot_en=1;
				PrintNumber(data_number);
			}
		}
////////////////////////////////////////////////////////////////////////////////////////////
//mode bom nito	
		if(key_pad==0x02){
			key_pad=0;
			mode_hut=0;mode_han=0;mode_lammat=0;mode_nito=1;mode_xa=0;
			if(program_en==1){
				buffer[0]=12;buffer[1]=10;buffer[2]=10;buffer[3]=10;buffer[4]=10;
				if(program_number==1){buffer[5]=data_store_1[3];buffer[6]=data_store_1[4];buffer[7]=data_store_1[5];}
				else if(program_number==2){buffer[5]=data_store_2[3];buffer[6]=data_store_2[4];buffer[7]=data_store_2[5];}
				else if(program_number==3){buffer[5]=data_store_3[3];buffer[6]=data_store_3[4];buffer[7]=data_store_3[5];}
				else if(program_number==4){buffer[5]=data_store_4[3];buffer[6]=data_store_4[4];buffer[7]=data_store_4[5];}
				else if(program_number==5){buffer[5]=data_store_5[3];buffer[6]=data_store_5[4];buffer[7]=data_store_5[5];}
				else if(program_number==6){buffer[5]=data_store_6[3];buffer[6]=data_store_6[4];buffer[7]=data_store_6[5];}
				else if(program_number==7){buffer[5]=data_store_7[3];buffer[6]=data_store_7[4];buffer[7]=data_store_7[5];}
				else if(program_number==8){buffer[5]=data_store_8[3];buffer[6]=data_store_8[4];buffer[7]=data_store_8[5];}
				else if(program_number==9){buffer[5]=data_store_9[3];buffer[6]=data_store_9[4];buffer[7]=data_store_9[5];}
				else if(program_number==10){buffer[5]=data_store_10[3];buffer[6]=data_store_10[4];buffer[7]=data_store_10[5];}	
				data_number=100*buffer[5]+10*buffer[6]+buffer[7];
				dot_en=1;
				PrintNumber(data_number);
			}
		}	
//mode han	
		if(key_pad==0x03){
			key_pad=0;
			mode_hut=0;mode_han=1;mode_lammat=0;mode_nito=0;mode_xa=0;
			if(program_en==1){
				buffer[0]=13;buffer[1]=10;buffer[2]=10;buffer[3]=10;buffer[4]=10;
				if(program_number==1){buffer[5]=data_store_1[6];buffer[6]=data_store_1[7];buffer[7]=data_store_1[8];}
				else if(program_number==2){buffer[5]=data_store_2[6];buffer[6]=data_store_2[7];buffer[7]=data_store_2[8];}
				else if(program_number==3){buffer[5]=data_store_3[6];buffer[6]=data_store_3[7];buffer[7]=data_store_3[8];}
				else if(program_number==4){buffer[5]=data_store_4[6];buffer[6]=data_store_4[7];buffer[7]=data_store_4[8];}
				else if(program_number==5){buffer[5]=data_store_5[6];buffer[6]=data_store_5[7];buffer[7]=data_store_5[8];}
				else if(program_number==6){buffer[5]=data_store_6[6];buffer[6]=data_store_6[7];buffer[7]=data_store_6[8];}
				else if(program_number==7){buffer[5]=data_store_7[6];buffer[6]=data_store_7[7];buffer[7]=data_store_7[8];}
				else if(program_number==8){buffer[5]=data_store_8[6];buffer[6]=data_store_8[7];buffer[7]=data_store_8[8];}
				else if(program_number==9){buffer[5]=data_store_9[6];buffer[6]=data_store_9[7];buffer[7]=data_store_9[8];}
				else if(program_number==10){buffer[5]=data_store_10[6];buffer[6]=data_store_10[7];buffer[7]=data_store_10[8];}	
				data_number=100*buffer[5]+10*buffer[6]+buffer[7];
				dot_en=1;
				PrintNumber(data_number);
			}
		}
//mode lam mat		
		if(key_pad==0x04){
			key_pad=0;
			mode_hut=0;mode_han=0;mode_lammat=1;mode_nito=0;mode_xa=0;
			if(program_en==1){
				buffer[0]=14;buffer[1]=10;buffer[2]=10;buffer[3]=10;buffer[4]=10;
				if(program_number==1){buffer[5]=data_store_1[9];buffer[6]=data_store_1[10];buffer[7]=data_store_1[11];}
				else if(program_number==2){buffer[5]=data_store_2[9];buffer[6]=data_store_2[10];buffer[7]=data_store_2[11];}
				else if(program_number==3){buffer[5]=data_store_3[9];buffer[6]=data_store_3[10];buffer[7]=data_store_3[11];}
				else if(program_number==4){buffer[5]=data_store_4[9];buffer[6]=data_store_4[10];buffer[7]=data_store_4[11];}
				else if(program_number==5){buffer[5]=data_store_5[9];buffer[6]=data_store_5[10];buffer[7]=data_store_5[11];}
				else if(program_number==6){buffer[5]=data_store_6[9];buffer[6]=data_store_6[10];buffer[7]=data_store_6[11];}
				else if(program_number==7){buffer[5]=data_store_7[9];buffer[6]=data_store_7[10];buffer[7]=data_store_7[11];}
				else if(program_number==8){buffer[5]=data_store_8[9];buffer[6]=data_store_8[10];buffer[7]=data_store_8[11];}
				else if(program_number==9){buffer[5]=data_store_9[9];buffer[6]=data_store_9[10];buffer[7]=data_store_9[11];}
				else if(program_number==10){buffer[5]=data_store_10[9];buffer[6]=data_store_10[10];buffer[7]=data_store_10[11];}	
				data_number=100*buffer[5]+10*buffer[6]+buffer[7];
				dot_en=1;
				PrintNumber(data_number);
			}
		}
//mode xa	
		if(key_pad==0x05){
			key_pad=0;
			mode_hut=0;mode_han=0;mode_lammat=0;mode_nito=0;mode_xa=1;
			if(program_en==1){
				buffer[0]=15;buffer[1]=10;buffer[2]=10;buffer[3]=10;buffer[4]=10;
				if(program_number==1){buffer[5]=data_store_1[12];buffer[6]=data_store_1[13];buffer[7]=data_store_1[14];}
				else if(program_number==2){buffer[5]=data_store_2[12];buffer[6]=data_store_2[13];buffer[7]=data_store_2[14];}
				else if(program_number==3){buffer[5]=data_store_3[12];buffer[6]=data_store_3[13];buffer[7]=data_store_3[14];}
				else if(program_number==4){buffer[5]=data_store_4[12];buffer[6]=data_store_4[13];buffer[7]=data_store_4[14];}
				else if(program_number==5){buffer[5]=data_store_5[12];buffer[6]=data_store_5[13];buffer[7]=data_store_5[14];}
				else if(program_number==6){buffer[5]=data_store_6[12];buffer[6]=data_store_6[13];buffer[7]=data_store_6[14];}
				else if(program_number==7){buffer[5]=data_store_7[12];buffer[6]=data_store_7[13];buffer[7]=data_store_7[14];}
				else if(program_number==8){buffer[5]=data_store_8[12];buffer[6]=data_store_8[13];buffer[7]=data_store_8[14];}
				else if(program_number==9){buffer[5]=data_store_9[12];buffer[6]=data_store_9[13];buffer[7]=data_store_9[14];}
				else if(program_number==10){buffer[5]=data_store_10[12];buffer[6]=data_store_10[13];buffer[7]=data_store_10[14];}	
				data_number=100*buffer[5]+10*buffer[6]+buffer[7];
				dot_en=1;
				PrintNumber(data_number);
			}
		}	
	}		
		if(key_pad=='i'){
			key_pad=0;
			if(mode_hut==1){
					data_number++;
				  dot_en=1;
					PrintNumber(data_number);
					update_data(0);			
			}
			if(mode_nito==1){
					data_number++;
				  dot_en=1;
					PrintNumber(data_number);
					update_data(3);			
			}
			if(mode_han==1){
					data_number++;
				  dot_en=1;
					PrintNumber(data_number);
					update_data(6);			
			}
			if(mode_lammat==1){
					data_number++;
				  dot_en=1;
					PrintNumber(data_number);
					update_data(9);			
			}
			if(mode_xa==1){
					data_number++;
				  dot_en=1;
					PrintNumber(data_number);
					update_data(12);			
			}
			if(program_en==1&&mode_hut==0&&mode_han==0&&mode_xa==0&&mode_nito==0&&mode_lammat==0){	
				program_number++;
				if(program_number>10)program_number=10;				
				if(program_number==1){buffer[0]=1;buffer[1]=93;}
				else if(program_number==2){buffer[0]=2;buffer[1]=93;}
				else if(program_number==3){buffer[0]=3;buffer[1]=93;}
				else if(program_number==4){buffer[0]=4;buffer[1]=93;}
				else if(program_number==5){buffer[0]=5;buffer[1]=93;}
				else if(program_number==6){buffer[0]=6;buffer[1]=93;}
				else if(program_number==7){buffer[0]=7;buffer[1]=93;}
				else if(program_number==8){buffer[0]=8;buffer[1]=93;}
				else if(program_number==9){buffer[0]=9;buffer[1]=93;}
				else if(program_number==10){buffer[0]=1;buffer[1]=0;}		
				buffer[2]=93;buffer[3]=93;buffer[4]=93;buffer[5]=93;buffer[6]=93;buffer[7]=93;	
			}
		}	

				

///////////////////////end increase button//////////////////////////////////////////////////////
//program button
		if(key_pad=='p'){
			key_pad=0;
			program_en++;
			if(program_en==2){
				while(EEPROM24XX_IsConnected()==0)HAL_Delay(100);	
				if(program_number==1)EEPROM24XX_Save(0,data_store_1,15);	
				else if(program_number==2)EEPROM24XX_Save(0x10,data_store_2,15);
				else if(program_number==3)EEPROM24XX_Save(0x20,data_store_3,15);
				else if(program_number==4)EEPROM24XX_Save(0x30,data_store_4,15);
				else if(program_number==5)EEPROM24XX_Save(0x40,data_store_5,15);
				else if(program_number==6)EEPROM24XX_Save(0x50,data_store_6,15);
				else if(program_number==7)EEPROM24XX_Save(0x60,data_store_7,15);
				else if(program_number==8)EEPROM24XX_Save(0x70,data_store_8,15);
				else if(program_number==9)EEPROM24XX_Save(0x80,data_store_9,15);
				else if(program_number==10)EEPROM24XX_Save(0x90,data_store_10,15);
				program_en=0;
				dot_en=0;
				mode_hut=0;mode_han=0;mode_lammat=0;mode_nito=0;mode_xa=0;
				buffer[0]=29;buffer[1]=30;buffer[2]=25;buffer[3]=26;buffer[4]=10;buffer[5]=10;buffer[6]=10;buffer[7]=10;
				HAL_Delay(1000);	
			}				
				if(program_en==1){
				if(program_number==1){buffer[0]=1;buffer[1]=93;}
				else if(program_number==2){buffer[0]=2;buffer[1]=93;}
				else if(program_number==3){buffer[0]=3;buffer[1]=93;}
				else if(program_number==4){buffer[0]=4;buffer[1]=93;}
				else if(program_number==5){buffer[0]=5;buffer[1]=93;}
				else if(program_number==6){buffer[0]=6;buffer[1]=93;}
				else if(program_number==7){buffer[0]=7;buffer[1]=93;}
				else if(program_number==8){buffer[0]=8;buffer[1]=93;}
				else if(program_number==9){buffer[0]=9;buffer[1]=93;}
				else if(program_number==10){buffer[0]=1;buffer[1]=0;}		
				buffer[2]=93;buffer[3]=93;buffer[4]=93;buffer[5]=93;buffer[6]=93;buffer[7]=93;					
		}
	}
////////////////end program button///////////////////////////////////////////////////////////
//decrease button	
		if(key_pad=='d'){
			key_pad=0;
		if(mode_hut==1){
					data_number--;
				dot_en=1;
					PrintNumber(data_number);
					update_data(0);
			}
		if(mode_nito==1){
					data_number--;
					dot_en=1;
					PrintNumber(data_number);
					update_data(3);
			}
		if(mode_han==1){
					data_number--;
					dot_en=1;
					PrintNumber(data_number);
					update_data(6);
			}
		if(mode_lammat==1){
					data_number--;
					dot_en=1;
					PrintNumber(data_number);
					update_data(9);
			}
		if(mode_xa==1){
					data_number--;
					dot_en=1;
					PrintNumber(data_number);
					update_data(12);
			}
			if(program_en==1&&mode_hut==0&&mode_han==0&&mode_xa==0&&mode_nito==0&&mode_lammat==0){								
				program_number--;
				if(program_number==0)program_number=1;	
				if(program_number==1){buffer[0]=1;buffer[1]=93;}
				else if(program_number==2){buffer[0]=2;buffer[1]=93;}
				else if(program_number==3){buffer[0]=3;buffer[1]=93;}
				else if(program_number==4){buffer[0]=4;buffer[1]=93;}
				else if(program_number==5){buffer[0]=5;buffer[1]=93;}
				else if(program_number==6){buffer[0]=6;buffer[1]=93;}
				else if(program_number==7){buffer[0]=7;buffer[1]=93;}
				else if(program_number==8){buffer[0]=8;buffer[1]=93;}
				else if(program_number==9){buffer[0]=9;buffer[1]=93;}
				else if(program_number==10){buffer[0]=1;buffer[1]=0;}		
				buffer[2]=93;buffer[3]=93;buffer[4]=93;buffer[5]=93;buffer[6]=93;buffer[7]=93;				
		}
	}
////////end decrease button////////////////////////////////////////////////////////////////////////				
		osDelay(150);
	}
///////////////////////////////////////////////////////////////////////////////////////////////////					
}
/* USER CODE END Header_start_reserve_isr */
void start_reserve_isr(void const * argument)
{
  for(;;)
  {
    if(kiemtra_ctht==1){
		while(EEPROM24XX_IsConnected()==0)HAL_Delay(100);	
		EEPROM24XX_Load((program_number-1)*16,data_load,15);
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
		dot_en=1;
		buffer[0]=11;buffer[1]=10;buffer[2]=10;buffer[3]=10;
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
		count_en=0;
		dot_en=0;
		buffer[0]=29;buffer[1]=30;buffer[2]=25;buffer[3]=26;buffer[4]=10;buffer[5]=10;buffer[6]=10;buffer[7]=10;
		HAL_Delay(1000);
		kiemtra_ctht=0;
		}
		osDelay(10);  
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
	buffer[5] = number % 1000 / 100;
	if(buffer[5]==0)buffer[5]=10;
	buffer[6] = number % 100 / 10;
	buffer[7] = number %10;
}
void ScreenUpdate(void)
{
	GPIOA->ODR=0x01<<digit;
	//GPIOC->ODR&=0x00ff;
	//GPIOC->ODR|=0x01<<(digit+8);
	if((digit==6)&&(dot_en==1)){GPIOC->ODR &= 0xff00; GPIOC->ODR |=(font[buffer[digit]]|0x80) & 0x00FF;}
	else {GPIOC->ODR &= 0xff00; GPIOC->ODR |=(font[buffer[digit]]) & 0x00FF;}
	digit++;
	if (digit > (SS_DIGIT-1))digit = 0;
}
void update_data(uint8_t mode){

if(program_number==1){
					data_store_1[mode]=data_number % 1000 / 100;
					data_store_1[mode+1]=data_number % 100 / 10;
					data_store_1[mode+2]=data_number % 10;
				}
				else if(program_number==2){
					data_store_2[mode]=data_number % 1000 / 100;
					data_store_2[mode+1]=data_number % 100 / 10;
					data_store_2[mode+2]=data_number % 10;
				}
				else if(program_number==3){
					data_store_3[mode]=data_number % 1000 / 100;
					data_store_3[mode+1]=data_number % 100 / 10;
					data_store_3[mode+2]=data_number % 10;
				}
				else if(program_number==4){
					data_store_4[mode]=data_number % 1000 / 100;
					data_store_4[mode+1]=data_number % 100 / 10;
					data_store_4[mode+2]=data_number % 10;
				}
				else if(program_number==5){
					data_store_5[mode]=data_number % 1000 / 100;
					data_store_5[mode+1]=data_number % 100 / 10;
					data_store_5[mode+2]=data_number % 10;
				}
				else if(program_number==6){
					data_store_6[mode]=data_number % 1000 / 100;
					data_store_6[mode+1]=data_number % 100 / 10;
					data_store_6[mode+2]=data_number % 10;
				}
				else if(program_number==7){
					data_store_7[mode]=data_number % 1000 / 100;
					data_store_7[mode+1]=data_number % 100 / 10;
					data_store_7[mode+2]=data_number % 10;
				}
				else if(program_number==8){
					data_store_8[mode]=data_number % 1000 / 100;
					data_store_8[mode+1]=data_number % 100 / 10;
					data_store_8[mode+2]=data_number % 10;
				}
				else if(program_number==9){
					data_store_9[mode]=data_number % 1000 / 100;
					data_store_9[mode+1]=data_number % 100 / 10;
					data_store_9[mode+2]=data_number % 10;
				}
				else if(program_number==10){
					data_store_10[mode]=data_number % 1000 / 100;
					data_store_10[mode+1]=data_number % 100 / 10;
					data_store_10[mode+2]=data_number % 10;
				}	
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
