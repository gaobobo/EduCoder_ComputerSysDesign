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
  HAL_Delay(100);
  OLED_RST_Clr();
  HAL_Delay(200);
  OLED_RST_Set();

//  OLED_WR_Byte(0xAE, OLED_CMD); //--turn off oled panel
//  OLED_WR_Byte(0x00, OLED_CMD); //---set low column address
//  OLED_WR_Byte(0x10, OLED_CMD); //---set high column address
//  OLED_WR_Byte(0x40, OLED_CMD); //--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
//  OLED_WR_Byte(0x81, OLED_CMD); //--set contrast control register
//  OLED_WR_Byte(0xCF, OLED_CMD); // Set SEG Output Current Brightness
//  OLED_WR_Byte(0xA1, OLED_CMD); //--Set SEG/Column Mapping     0xa0左右反置 0xa1正常
//  OLED_WR_Byte(0xC8, OLED_CMD); // Set COM/Row Scan Direction   0xc0上下反置 0xc8正常
//  OLED_WR_Byte(0xA6, OLED_CMD); //--set normal display
//  OLED_WR_Byte(0xA8, OLED_CMD); //--set multiplex ratio(1 to 64)
//  OLED_WR_Byte(0x3f, OLED_CMD); //--1/64 duty
//  OLED_WR_Byte(0xD3, OLED_CMD); //-set display offset	Shift Mapping RAM Counter (0x00~0x3F)
//  OLED_WR_Byte(0x00, OLED_CMD); //-not offset
//  OLED_WR_Byte(0xd5, OLED_CMD); //--set display clock divide ratio/oscillator frequency
//  OLED_WR_Byte(0x80, OLED_CMD); //--set divide ratio, Set Clock as 100 Frames/Sec
//  OLED_WR_Byte(0xD9, OLED_CMD); //--set pre-charge period
//  OLED_WR_Byte(0xF1, OLED_CMD); // Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
//  OLED_WR_Byte(0xDA, OLED_CMD); //--set com pins hardware configuration
//  OLED_WR_Byte(0x12, OLED_CMD);
//  OLED_WR_Byte(0xDB, OLED_CMD); //--set vcomh
//  OLED_WR_Byte(0x40, OLED_CMD); // Set VCOM Deselect Level
//  OLED_WR_Byte(0x20, OLED_CMD); //-Set Page Addressing Mode (0x00/0x01/0x02)
//  OLED_WR_Byte(0x02, OLED_CMD); //
//  OLED_WR_Byte(0x8D, OLED_CMD); //--set Charge Pump enable/disable
//  OLED_WR_Byte(0x14, OLED_CMD); //--set(0x10) disable
//  OLED_WR_Byte(0xA4, OLED_CMD); // Disable Entire Display On (0xa4/0xa5)
//  OLED_WR_Byte(0xA6, OLED_CMD); // Disable Inverse Display On (0xa6/a7)
//  OLED_WR_Byte(0xAF, OLED_CMD); //--turn on oled panel

//  OLED_WR_Byte(0xAE,OLED_CMD); //关闭显示
//  OLED_WR_Byte(0xD5,OLED_CMD); //设置时钟分频因子,震荡频率
//  OLED_WR_Byte(80,OLED_CMD);   //[3:0],分频因子;[7:4],震荡频率
//  OLED_WR_Byte(0xA8,OLED_CMD); //设置驱动路数
//  OLED_WR_Byte(0X3F,OLED_CMD); //默认0X3F(1/64)
//  OLED_WR_Byte(0xD3,OLED_CMD); //设置显示偏移
//  OLED_WR_Byte(0X00,OLED_CMD); //默认为0
//  OLED_WR_Byte(0x40,OLED_CMD); //设置显示开始行 [5:0],行数.
//  OLED_WR_Byte(0x8D,OLED_CMD); //电荷泵设置
//  OLED_WR_Byte(0x14,OLED_CMD); //bit2，开启/关闭
//  OLED_WR_Byte(0x20,OLED_CMD); //设置内存地址模式
//  OLED_WR_Byte(0x10,OLED_CMD); //[1:0],00，列地址模式;01，行地址模式;10,页地址模式;默认10;
//  OLED_WR_Byte(0xA1,OLED_CMD); //段重定义设置,bit0:0,0->0;1,0->127;
//  OLED_WR_Byte(0xC0,OLED_CMD); //设置COM扫描方向;bit3:0,普通模式;1,重定义模式 COM[N-1]->COM0;N:驱动路数
//  OLED_WR_Byte(0xDA,OLED_CMD); //设置COM硬件引脚配置
//  OLED_WR_Byte(0x12,OLED_CMD); //[5:4]配置
//  OLED_WR_Byte(0x81,OLED_CMD); //对比度设置
//  OLED_WR_Byte(0xEF,OLED_CMD); //1~255;默认0X7F (亮度设置,越大越亮)
//  OLED_WR_Byte(0xD9,OLED_CMD); //设置预充电周期
//  OLED_WR_Byte(0xf1,OLED_CMD); //[3:0],PHASE 1;[7:4],PHASE 2;
//  OLED_WR_Byte(0xDB,OLED_CMD); //设置VCOMH 电压倍率
//  OLED_WR_Byte(0x30,OLED_CMD); //[6:4] 000,0.65*vcc;001,0.77*vcc;011,0.83*vcc;
//  OLED_WR_Byte(0xA4,OLED_CMD); //全局显示开启;bit0:1,开启;0,关闭;(白屏/黑屏)
//  OLED_WR_Byte(0xA6,OLED_CMD); //设置显示方式;bit0:1,反相显示;0,正常显示
//  OLED_WR_Byte(0xAF,OLED_CMD); //开启显示

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

  OLED_Clear();
  OLED_Set_Pos(0, 0);
}

void testFun()
{
  OLED_Set_Pos(0, 0);
  for (size_t i = 0; i < 128; i++)
  {
    OLED_WR_Byte(0xFF, OLED_DATA);
    HAL_Delay(5);
  }

  OLED_Set_Pos(0, 1);
  for (size_t i = 0; i < 128; i++)
  {
    OLED_WR_Byte(0xFF, OLED_DATA);
    HAL_Delay(5);
  }

  OLED_Set_Pos(0, 2);
  uint8_t bData = 0x0F;
  for (size_t i = 0; i < 1024; i++)
  {
	if (i % 128 == 0) bData++;
    OLED_WR_Byte(bData, OLED_DATA);
    HAL_Delay(5);
  }

  OLED_Set_Pos(0, 3);
  for (size_t i = 0; i < 16; i++)
  {
    OLED_WR_Byte(0xFF, OLED_DATA);
    HAL_Delay(5);
  }
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
  OLED_Clear();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  printf("hello, stm32f103rbt6\r\n");

//  uint8_t t = ' ';
//  OLED_ShowCHinese(0, 0, 0);
//  OLED_ShowCHinese(18, 0, 1);
//  OLED_ShowCHinese(36, 0, 2);
//  OLED_ShowCHinese(54, 0, 3);
//  OLED_ShowCHinese(72, 0, 4);
//  OLED_ShowCHinese(90, 0, 5);
//  OLED_ShowCHinese(108, 0, 6);
  OLED_ShowString(0, 0, (uint8_t *)"YUNFENG - OLED");
  OLED_ShowString(0, 2, (uint8_t *)"abcdefg123456789");
  OLED_ShowString(0, 4, (uint8_t *)__DATE__);
  OLED_ShowString(0, 6, (uint8_t *)__TIME__);
//  OLED_ShowString(0, 3, (uint8_t *)"1.3' OLED TEST");
//  OLED_ShowString(0, 6, (uint8_t *)"ASCII:");
//  OLED_ShowString(63, 6, (uint8_t *)"CODE:");
  HAL_Delay(30000);

  testFun();
  HAL_Delay(5000);

  uint8_t bScreenWidth = 128;
  uint16_t wShowCharCnt = 0;
  uint8_t bData = 0x01;
  while (1)
  {
    //    OLED_ShowChar(48, 6, t);
    //    t++;
    //    if (t > '~')
    //      t = ' ';
    //    OLED_ShowNum(103, 6, t, 3, 16);

	uint8_t bX = wShowCharCnt % bScreenWidth;
	uint8_t bY = wShowCharCnt / bScreenWidth;

	if (bY > 7)
    {
      bData++;
      printf("new screen, wShowCharCnt=%d, bData=%d\r\n",
    		  wShowCharCnt, bData);
      wShowCharCnt = 0;

  	  bX = wShowCharCnt % bScreenWidth;
  	  bY = wShowCharCnt / bScreenWidth;
    }

    if (bX == 0)
    {
      OLED_Set_Pos(0, bY);
      printf("enter line, x=%d, y=%d\r\n", bX, bY);
    }
    OLED_WR_Byte(bData, OLED_DATA);
    wShowCharCnt++;
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
    HAL_Delay(5);
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
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
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
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4|GPIO_PIN_6|GPIO_PIN_7|LED_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : PB4 PB6 PB7 LED_Pin */
  GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_6|GPIO_PIN_7|LED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

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
  printf("Wrong parameters value: file %s on line %ld\r\n", file, line);
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
