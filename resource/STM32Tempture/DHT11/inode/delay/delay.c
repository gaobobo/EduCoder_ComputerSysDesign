/*
 * delay.c
 *
 *  Created on: Feb 25, 2025
 *      Author: happy
 */


 #include "delay.h"

 void delay_us(uint32_t us)
 {
     uint32_t delay = (HAL_RCC_GetHCLKFreq() / 8000000 * us); 
     while (delay--);
 }
 