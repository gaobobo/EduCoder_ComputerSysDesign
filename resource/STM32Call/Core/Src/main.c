/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2023 STMicroelectronics.
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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <stdarg.h>
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
SPI_HandleTypeDef hspi1;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
uint8_t g_bKeyValue = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_SPI1_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
int _write(int fd, char *ch, int len)
{
  HAL_UART_Transmit(&huart2, (uint8_t *)ch, len, 0xFFFF);
  return len;
}

int u_printf(const char *format, ...)
{
	char loc_buf[256];
	va_list arg;
	va_list copy;
	va_start(arg, format);
	va_copy(copy, arg);
	int len = vsnprintf(loc_buf, sizeof(loc_buf), format, copy);
	va_end(copy);
	va_end(arg);

	if (len > 0 && len <= 256)
	{
		HAL_UART_Transmit(&huart2, (uint8_t *)loc_buf, len, 0xFF);
	}
	return len;
}

#include "key.h"
#include "bmp.h"
#include "oled.h"
#include "stdlib.h"
#include "oledfont.h"
// OLED的显存
// 存放格式如下.
//[0]0 1 2 3 ... 127
//[1]0 1 2 3 ... 127
//[2]0 1 2 3 ... 127
//[3]0 1 2 3 ... 127
//[4]0 1 2 3 ... 127
//[5]0 1 2 3 ... 127
//[6]0 1 2 3 ... 127
//[7]0 1 2 3 ... 127

#if OLED_MODE == 1
// 向SSD1106写入一个字节。
// dat:要写入的数据/命令
// cmd:数据/命令标志 0,表示命令;1,表示数据;
void OLED_WR_Byte(u8 dat, u8 cmd)
{
  DATAOUT(dat);
  if (cmd)
    OLED_DC_Set();
  else
    OLED_DC_Clr();
  OLED_CS_Clr();
  OLED_WR_Clr();
  OLED_WR_Set();
  OLED_CS_Set();
  OLED_DC_Set();
}
#else
// 向SSD1106写入一个字节。
// dat:要写入的数据/命令
// cmd:数据/命令标志 0,表示命令;1,表示数据;
void OLED_WR_Byte(u8 dat, u8 cmd)
{
  if (cmd)
    OLED_DC_Set();
  else
    OLED_DC_Clr();
  OLED_CS_Clr();
  HAL_SPI_Transmit(&hspi1, &dat, 1, 0xFF);
//  for (u8 i = 0; i < 8; i++)
//  {
//    OLED_SCLK_Clr();
//    if (dat & 0x80)
//      OLED_SDIN_Set();
//    else
//      OLED_SDIN_Clr();
//    OLED_SCLK_Set();
//    dat <<= 1;
//  }
  OLED_CS_Set();
  OLED_DC_Set();
}
#endif

void OLED_Set_Pos(unsigned char x, unsigned char y)
{
  OLED_WR_Byte(0xb0 + y, OLED_CMD); // 设置页地址（0~7）
  OLED_WR_Byte(x & 0x0f, OLED_CMD); // 设置列地址的低地址
  OLED_WR_Byte(((x & 0xf0) >> 4) | 0x10, OLED_CMD); // 设置列地址的高地址
}

// 功能说明: 设置显示方向 _ucDir = 0 表示正常方向，1表示翻转180度
void OLED_SetDir(uint8_t _ucDir)
{
	if (_ucDir == 0)
	{
		OLED_WR_Byte(0xA0, OLED_CMD);	/* A0 ：列地址0映射到SEG0; A1 ：列地址127映射到SEG0 */
		OLED_WR_Byte(0xC0, OLED_CMD);	/* C0 ：正常扫描,从COM0到COM63;  C8 : 反向扫描, 从 COM63至 COM0 */
	}
	else
	{
		OLED_WR_Byte(0xA1, OLED_CMD);	/* A0 ：列地址0映射到SEG0; A1 ：列地址127映射到SEG0 */
		OLED_WR_Byte(0xC8, OLED_CMD);	/* C0 ：正常扫描,从COM0到COM63;  C8 : 反向扫描, 从 COM63至 COM0 */
	}
}

// 开启OLED显示
void OLED_Display_On(void)
{
  OLED_WR_Byte(0X8D, OLED_CMD); // SET DCDC命令
  OLED_WR_Byte(0X14, OLED_CMD); // DCDC ON
  OLED_WR_Byte(0XAF, OLED_CMD); // DISPLAY ON
}
// 关闭OLED显示
void OLED_Display_Off(void)
{
  OLED_WR_Byte(0X8D, OLED_CMD); // SET DCDC命令
  OLED_WR_Byte(0X10, OLED_CMD); // DCDC OFF
  OLED_WR_Byte(0XAE, OLED_CMD); // DISPLAY OFF
}

//屏幕旋转180度
void OLED_DisplayTurn(uint8_t i)
{
	if(i==0)
		{
			OLED_WR_Byte(0xC8,OLED_CMD);//正常显示
			OLED_WR_Byte(0xA1,OLED_CMD);
		}
	if(i==1)
		{
			OLED_WR_Byte(0xC0,OLED_CMD);//反转显示
			OLED_WR_Byte(0xA0,OLED_CMD);
		}
}

// 清屏函数,清完屏,整个屏幕是黑色的!和没点亮一样!!!
void OLED_Clear(void)
{
  u8 i, n;
  for (i = 0; i < 8; i++)
  {
    OLED_WR_Byte(0xb0 + i, OLED_CMD); // 设置页地址（0~7）
    OLED_WR_Byte(0x00, OLED_CMD);     // 设置显示位置—列低地址
    OLED_WR_Byte(0x10, OLED_CMD);     // 设置显示位置—列高地址
    for (n = 0; n < 128; n++)
      OLED_WR_Byte(0, OLED_DATA);
  } // 更新显示
}

// 在指定位置显示一个字符,包括部分字符
// x:0~127
// y:0~63
// mode:0,反白显示;1,正常显示
// size:选择字体 16/12
void OLED_ShowChar(u8 x, u8 y, u8 chr)
{
  unsigned char c = 0, i = 0;
  c = chr - ' '; // 得到偏移后的值
  if (x > Max_Column - 1)
  {
    x = 0;
    y = y + 2;
  }
  if (SIZE == 16)
  {
    OLED_Set_Pos(x, y);
    for (i = 0; i < 8; i++)
      OLED_WR_Byte(F8X16[c * 16 + i], OLED_DATA);
    OLED_Set_Pos(x, y + 1);
    for (i = 0; i < 8; i++)
      OLED_WR_Byte(F8X16[c * 16 + i + 8], OLED_DATA);
  }
  else
  {
    OLED_Set_Pos(x, y + 1);
    for (i = 0; i < 6; i++)
      OLED_WR_Byte(F6x8[c][i], OLED_DATA);
  }
}
// m^n函数
u32 oled_pow(u8 m, u8 n)
{
  u32 result = 1;
  while (n--)
    result *= m;
  return result;
}
// 显示2个数字
// x,y :起点坐标
// len :数字的位数
// size:字体大小
// mode:模式	0,填充模式;1,叠加模式
// num:数值(0~4294967295);
void OLED_ShowNum(u8 x, u8 y, u32 num, u8 len, u8 size)
{
  u8 t, temp;
  u8 enshow = 0;
  for (t = 0; t < len; t++)
  {
    temp = (num / oled_pow(10, len - t - 1)) % 10;
    if (enshow == 0 && t < (len - 1))
    {
      if (temp == 0)
      {
        OLED_ShowChar(x + (size / 2) * t, y, ' ');
        continue;
      }
      else
        enshow = 1;
    }
    OLED_ShowChar(x + (size / 2) * t, y, temp + '0');
  }
}
// 显示一个字符号串
void OLED_ShowString(u8 x, u8 y, u8 *chr)
{
  unsigned char j = 0;
  while (chr[j] != '\0')
  {
    OLED_ShowChar(x, y, chr[j]);
    x += 8;
    if (x > 120)
    {
      x = 0;
      y += 2;
    }
    j++;
  }
}
// 显示汉字
void OLED_ShowCHinese(u8 x, u8 y, u8 no)
{
  u8 t, adder = 0;
  OLED_Set_Pos(x, y);
  for (t = 0; t < 16; t++)
  {
    OLED_WR_Byte(Hzk[2 * no][t], OLED_DATA);
    adder += 1;
  }
  OLED_Set_Pos(x, y + 1);
  for (t = 0; t < 16; t++)
  {
    OLED_WR_Byte(Hzk[2 * no + 1][t], OLED_DATA);
    adder += 1;
  }
}
/***********功能描述：显示显示BMP图片128×64起始点坐标(x,y),x的范围0～127，y为页的范围0～7*****************/
void OLED_DrawBMP(unsigned char x0, unsigned char y0, unsigned char x1, unsigned char y1, unsigned char BMP[])
{
  unsigned int j = 0;
  unsigned char x, y;

  if (y1 % 8 == 0)
    y = y1 / 8;
  else
    y = y1 / 8 + 1;
  for (y = y0; y < y1; y++)
  {
    OLED_Set_Pos(x0, y);
    for (x = x0; x < x1; x++)
    {
      OLED_WR_Byte(BMP[j++], OLED_DATA);
    }
  }
}

/* 下面2个宏任选 1个; 表示显示方向 */
//#define DIR_NORMAL			/* 此行表示正常显示方向 */
#define DIR_180				/* 此行表示翻转180度 */

// 初始化SSD1306
void OLED_Init(void)
{
  OLED_RST_Set();

  // 仿真：初始化引脚值
  OLED_DC_Set();
  OLED_CS_Set();
  OLED_SCLK_Set();
  OLED_SDIN_Clr();

  HAL_Delay(100);
  OLED_RST_Clr();
  HAL_Delay(200);
  OLED_RST_Set();

  /* 模块厂家提供初始化代码 */
  OLED_WR_Byte(0xAE, OLED_CMD); /* 关闭OLED面板显示(休眠) */
  OLED_WR_Byte(0x00, OLED_CMD); /* 设置列地址低4bit */
  OLED_WR_Byte(0x10, OLED_CMD); /* 设置列地址高4bit */
  OLED_WR_Byte(0x40, OLED_CMD); /* 设置起始行地址（低5bit 0-63）， 硬件相关*/
  OLED_WR_Byte(0x81, OLED_CMD); /* 设置对比度命令(双字节命令），第1个字节是命令，第2个字节是对比度参数0-255 */
  OLED_WR_Byte(0xCF, OLED_CMD); /* 设置对比度参数,缺省CF */
#ifdef DIR_NORMAL
  OLED_WR_Byte(0xA0, OLED_CMD); /* A0 ：列地址0映射到SEG0; A1 ：列地址127映射到SEG0 */
  OLED_WR_Byte(0xC0, OLED_CMD); /* C0 ：正常扫描,从COM0到COM63;  C8 : 反向扫描, 从 COM63至 COM0 */
#endif
#ifdef DIR_180
  OLED_WR_Byte(0xA1, OLED_CMD); /* A0 ：列地址0映射到SEG0; A1 ：列地址127映射到SEG0 */
  OLED_WR_Byte(0xC8, OLED_CMD); /* C0 ：正常扫描,从COM0到COM63;  C8 : 反向扫描, 从 COM63至 COM0 */
#endif
  OLED_WR_Byte(0xA6, OLED_CMD); /* A6 : 设置正常显示模式; A7 : 设置为反显模式 */
  OLED_WR_Byte(0xA8, OLED_CMD); /* 设置COM路数 */
  OLED_WR_Byte(0x3F, OLED_CMD); /* 1 ->（63+1）路 */
  OLED_WR_Byte(0xD3, OLED_CMD); /* 设置显示偏移（双字节命令）*/
  OLED_WR_Byte(0x00, OLED_CMD); /* 无偏移 */
  OLED_WR_Byte(0xD5, OLED_CMD); /* 设置显示时钟分频系数/振荡频率 */
  OLED_WR_Byte(0x80, OLED_CMD); /* 设置分频系数,高4bit是分频系数，低4bit是振荡频率 */
  OLED_WR_Byte(0xD9, OLED_CMD); /* 设置预充电周期 */
  OLED_WR_Byte(0xF1, OLED_CMD); /* [3:0],PHASE 1; [7:4],PHASE 2; */
  OLED_WR_Byte(0xDA, OLED_CMD); /* 设置COM脚硬件接线方式 */
  OLED_WR_Byte(0x12, OLED_CMD);
  OLED_WR_Byte(0xDB, OLED_CMD); /* 设置 vcomh 电压倍率 */
  OLED_WR_Byte(0x40, OLED_CMD); /* [6:4] 000 = 0.65 x VCC; 0.77 x VCC (RESET); 0.83 x VCC  */
  OLED_WR_Byte(0x8D, OLED_CMD); /* 设置充电泵（和下个命令结合使用） */
  OLED_WR_Byte(0x14, OLED_CMD); /* 0x14 使能充电泵， 0x10 是关闭 */
  OLED_WR_Byte(0xAF, OLED_CMD); /* 打开OLED面板 */

  // OLED_Clear();
  // OLED_Set_Pos(0, 0);
}

void pixelTestFun()
{
	// 第一行，每行占8个像素点高，总共八行
  OLED_Set_Pos(0, 0);
  for (size_t i = 0; i < 128; i++)
  {
    OLED_WR_Byte(0xAA, OLED_DATA);
    HAL_Delay(30);
  }

	// 第二行，总共八行
  OLED_Set_Pos(0, 1);
  for (size_t i = 0; i < 128; i++)
  {
    OLED_WR_Byte(0xFF, OLED_DATA);
    HAL_Delay(30);
  }

	// 第三行，总共八行
  OLED_Set_Pos(0, 2);
  for (size_t i = 0; i < 128; i++)
  {
    OLED_WR_Byte(0x0F, OLED_DATA);
    HAL_Delay(30);
  }

	// 第四行，总共八行
  OLED_Set_Pos(0, 3);
  for (size_t i = 0; i < 128; i++)
  {
    OLED_WR_Byte(0x55, OLED_DATA);
    HAL_Delay(30);
  }
}

uint8_t getExtiKeyValue()
{
  uint8_t bKey = g_bKeyValue;
  g_bKeyValue = 0;
  return bKey;
}

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
  MX_USART2_UART_Init();
  MX_SPI1_Init();
  /* USER CODE BEGIN 2 */
  OLED_Init();
  OLED_DisplayTurn(1);
  // OLED_Clear();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  u_printf("hello, stm32f103rbt6, %f\r\n", 1.234);

  // 静态字符显示
  //OLED_ShowString(0, 0, (uint8_t *)"YUNFENG - OLED");
  //OLED_ShowString(0, 2, (uint8_t *)"abcdefg123456789");
  //OLED_ShowString(0, 4, (uint8_t *)__DATE__);
  //OLED_ShowString(0, 6, (uint8_t *)__TIME__);
  //HAL_Delay(5000);

  // 像素点测试
  // pixelTestFun();
  // HAL_Delay(5000);

  // 清屏，填充黑色
  OLED_Clear();

	// key_Config();
  unsigned char *oledBmpArray[] = {
                                  yunFengLogoWordHorizontal12864Bmp,
                                  yunFengLogoWordVertical12864Bmp,
                                 };
  unsigned char oledBmpIndex = 0;
  // 默认打开显示屏和灯，并且显示1次数据
  uint8_t bSendDataEn = 1;
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    int userInput = getExtiKeyValue();
    // 非0值代表任意按键被按下
    if (userInput)
    {
      /**********begin**********/

      // 任意按键被按下，显示屏发送数据使能    
      // bSendDataEn ...
      //////////////////////////////////////////////////////////////////// PS: The answer is as follows:
      
      bSendDataEn = 1;

      ////////////////////////////////////////////////////////////////////// PS: End of question answer.
      
      /***********end**********/

    }

    if (userInput == 1) // 对应板载KEY1，切换到上张图片
    {
        oledBmpIndex --;
        oledBmpIndex %= sizeof(oledBmpArray) / sizeof(oledBmpArray[0]);

    }
    else if (userInput == 2)
    {
        oledBmpIndex ++;
        oledBmpIndex %= sizeof(oledBmpArray) / sizeof(oledBmpArray[0]);
    }

    // 显示屏数据只发送一次，否则会占CPU，影响中断接收
    if (bSendDataEn)
    {
      bSendDataEn = 0;
      // 显示屏刷新
      OLED_DrawBMP(0, 0, 128, 8, oledBmpArray[oledBmpIndex]);
      HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
    }
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

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
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
  hspi1.Init.Direction = SPI_DIRECTION_1LINE;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
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
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

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
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4|GPIO_PIN_6|GPIO_PIN_7|LED_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : PC14 PC15 */
  GPIO_InitStruct.Pin = GPIO_PIN_14|GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PB4 PB6 PB7 LED_Pin */
  GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_6|GPIO_PIN_7|LED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

}

/* USER CODE BEGIN 4 */
// 按键中断回调
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if (GPIO_Pin == GPIO_PIN_14)
	{
		g_bKeyValue = 1;
	}
	else if (GPIO_Pin == GPIO_PIN_15)
	{
		g_bKeyValue = 2;
	}
	u_printf("exti signal %x\r\n", GPIO_Pin);
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
  u_printf("Wrong parameters value: file %s on line %u\r\n", file, line);
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
