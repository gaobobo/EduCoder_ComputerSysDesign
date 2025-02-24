/*
 * lcd_init.h
 *
 *  Created on: 2025年2月24日
 *      Author: happy
 */

#ifndef INC_LCD_INIT_H_
#define INC_LCD_INIT_H_


#include "stm32f1xx_hal.h"

#define USE_HORIZONTAL 1 // 设置横屏或者竖屏显示 0或1为竖屏 2或3为横屏

#if USE_HORIZONTAL == 0 || USE_HORIZONTAL == 1
#define LCD_W 128
#define LCD_H 160

#else
#define LCD_W 160
#define LCD_H 128
#endif

#define u8 uint8_t
#define u16 uint16_t
#define u32 uint32_t

//-----------------LCD端口定义----------------

#define LCD_SCLK_Clr() HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_RESET) // SCL=SCLK
#define LCD_SCLK_Set() HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_SET)

#define LCD_MOSI_Clr() HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET) // SDA=MOSI
#define LCD_MOSI_Set() HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET)

#define LCD_RES_Clr() HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_RESET) // RES
#define LCD_RES_Set() HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_SET)

#define LCD_DC_Clr() HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_RESET) // DC
#define LCD_DC_Set() HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_SET)

#define LCD_CS_Clr() HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET) // CS
#define LCD_CS_Set() HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET)

#define LCD_BLK_Clr() HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET) // BLK
#define LCD_BLK_Set() HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET)

void LCD_GPIO_Init(void);                             // 初始化GPIO
void LCD_Writ_Bus(u8 dat);                            // 模拟SPI时序
void LCD_WR_DATA8(u8 dat);                            // 写入一个字节
void LCD_WR_DATA(u16 dat);                            // 写入两个字节
void LCD_WR_REG(u8 dat);                              // 写入一个指令
void LCD_Address_Set(u16 x1, u16 y1, u16 x2, u16 y2); // 设置坐标函数
void LCD_Init(void);                                  // LCD初始化
#endif
