/*
 * dht11.h
 *
 *  Created on: 2025年2月26日
 *      Author: happy
 */

#ifndef INC_DHT11_H_
#define INC_DHT11_H_

#include "stm32f1xx_hal.h"
#include "delay.h"
void DHT11_IO_OUT (void);
void DHT11_IO_IN (void);
void DHT11_RST (void);
uint8_t Dht11_Check(void);
uint8_t Dht11_ReadBit(void);
uint8_t Dht11_ReadByte(void);
uint8_t DHT11_Init (void);
uint8_t DHT11_ReadData(uint8_t *h);

#endif /* INC_DHT11_H_ */
