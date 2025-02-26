/*
 * delay.c
 *
 *  Created on: 2025年2月26日
 *      Author: happy
 */


#include "delay.h"

void delay_us(uint32_t us)
{
    uint32_t delay = (HAL_RCC_GetHCLKFreq() / 8000000 * us);
    while (delay--);
}
