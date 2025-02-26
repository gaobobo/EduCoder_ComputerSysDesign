#ifndef __OLED_H
#define __OLED_H
#include "stm32f1xx_hal.h"
#include "stdlib.h"
// OLED模式设置
// 0:4线串行模式
// 1:并行8080模式
#define OLED_MODE 0
#define SIZE 16
#define XLevelL 0x00
#define XLevelH 0x10
#define Max_Column 128
#define Max_Row 64
#define Brightness 0xFF
#define X_WIDTH 128
#define Y_WIDTH 64

#define OLED_IO_DELAY 0 // ms

//-----------------OLED端口定义----------------
#if OLED_IO_DELAY == 0

#define OLED_SCLK_Clr() HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_RESET) // CLK/D0
#define OLED_SCLK_Set() HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_SET)

#define OLED_SDIN_Clr() HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_RESET) // DIN/D1
#define OLED_SDIN_Set() HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_SET)

#define OLED_RST_Clr() HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_RESET) // RES
#define OLED_RST_Set() HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_SET)

#define OLED_DC_Clr() HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET) // DC
#define OLED_DC_Set() HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET)

#define OLED_CS_Clr() HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_RESET) // CS
#define OLED_CS_Set() HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_SET)

#else

#define OLED_SCLK_Clr() do { HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_RESET); HAL_Delay(OLED_IO_DELAY); } while(0) // CLK/D0
#define OLED_SCLK_Set() do { HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_SET); HAL_Delay(OLED_IO_DELAY); } while(0)

#define OLED_SDIN_Clr() do { HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_RESET); HAL_Delay(OLED_IO_DELAY); } while(0) // DIN/D5
#define OLED_SDIN_Set() do { HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_SET); HAL_Delay(OLED_IO_DELAY); } while(0)

#define OLED_RST_Clr() do { HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_RESET); HAL_Delay(OLED_IO_DELAY); } while(0) // RES
#define OLED_RST_Set() do { HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_SET); HAL_Delay(OLED_IO_DELAY); } while(0)

#define OLED_DC_Clr() do { HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET); HAL_Delay(OLED_IO_DELAY); } while(0) // DC
#define OLED_DC_Set() do { HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET); HAL_Delay(OLED_IO_DELAY); } while(0)

#define OLED_CS_Clr() do { HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_RESET); HAL_Delay(OLED_IO_DELAY); } while(0) // CS
#define OLED_CS_Set() do { HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_SET); HAL_Delay(OLED_IO_DELAY); } while(0)

#endif

#define OLED_CMD 0  // 写命令
#define OLED_DATA 1 // 写数据

#define u8 uint8_t
#define u16 uint16_t
#define u32 uint32_t

// OLED控制用函数
void OLED_WR_Byte(u8 dat, u8 cmd);
void OLED_Display_On(void);
void OLED_Display_Off(void);
void OLED_DisplayTurn(uint8_t i);
void OLED_Init(void);
void OLED_Clear(void);
void OLED_DrawPoint(u8 x, u8 y, u8 t);
void OLED_Fill(u8 x1, u8 y1, u8 x2, u8 y2, u8 dot);
void OLED_ShowChar(u8 x, u8 y, u8 chr);
void OLED_ShowNum(u8 x, u8 y, u32 num, u8 len, u8 size);
void OLED_ShowString(u8 x, u8 y, u8 *p);
void OLED_Set_Pos(unsigned char x, unsigned char y);
void OLED_ShowCHinese(u8 x, u8 y, u8 no);
void OLED_DrawBMP(unsigned char x0, unsigned char y0, unsigned char x1, unsigned char y1, unsigned char BMP[]);
#endif
