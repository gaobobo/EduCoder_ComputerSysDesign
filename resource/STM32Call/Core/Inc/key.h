#ifndef __key_h
#define __key_h
 
 
//#include  "stm32f10x.h"
#include "stm32f1xx_hal.h"
//#include "main.h"
//#include  "delay.h"
//#include  "LED.h"
// 
 

#define KEY1_Pin GPIO_PIN_14
#define KEY1_GPIO_Port GPIOC

#define KEY2_Pin GPIO_PIN_15
#define KEY2_GPIO_Port GPIOC

#define EXT_KEY_GND_Pin GPIO_PIN_15
#define EXT_KEY_GND_GPIO_Port GPIOA

#define EXT_KEY1_Pin GPIO_PIN_10
#define EXT_KEY1_GPIO_Port GPIOC

#define EXT_KEY2_Pin GPIO_PIN_11
#define EXT_KEY2_GPIO_Port GPIOC

#define EXT_KEY3_Pin GPIO_PIN_12
#define EXT_KEY3_GPIO_Port GPIOC

#define EXT_KEY4_Pin GPIO_PIN_2
#define EXT_KEY4_GPIO_Port GPIOD

#define LED_Pin_1 GPIO_PIN_8  //����� LED����
#define LED_GPIO_Port GPIOB

//#define LED_Pin_2 GPIO_PIN_9
//#define LED_GPIO_Port GPIOB


 
 
 
//���� ״̬
//#define  KEY_1  GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0) 
#define KEY_1 HAL_GPIO_ReadPin(KEY1_GPIO_Port, KEY1_Pin)  //PC14
 
//#define  KEY_2  GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_1)
#define KEY_2 HAL_GPIO_ReadPin(KEY2_GPIO_Port, KEY2_Pin)     //  //PC15 ��ȡ��ֵ ����Ϊ0 
 
//#define  KEY_3  GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_2)
#define KEY_3 HAL_GPIO_ReadPin(EXT_KEY1_GPIO_Port, EXT_KEY1_Pin)

#define KEY_4 HAL_GPIO_ReadPin(EXT_KEY2_GPIO_Port, EXT_KEY2_Pin)
 
 
//����ֵ 
#define KEY1_PRES 	1	//KEY0����
 
#define KEY2_PRES	  2	//KEY1����
 
#define KEY3_PRES   3	//KEY3����

#define KEY4_PRES   4	//KEY4���� ����ֵ

#define LONG_PRES 2   // ����������ֵ

# define DOUBLE_PRES 3    // ˫��������ֵ




 
 
//���� ����
void key_Config(void);			//���� ���ذ���
 
uint8_t KEY_Scanf(uint8_t mode);			//���� ɨ��
 
void key_function(void);		//���� ����
 
 
#endif