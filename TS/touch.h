/*
 * touch.h
 *
 *  Created on: May 25 2020
 *      Author: Alex
 *
 *  The TFT touch panel driver based on XPT2046 IC
 *  Connect the display by the following way:
 *  display	STM32
 *  T_IRQ	TOUCH_IRQ
 *  T_DO	MISO
 *  T_DIN	MOSI
 *  T_CS	TOUCH_CS
 *  T_CLK	SCK
 *
 */

#ifndef _TOUCH_H_
#define _TOUCH_H_

#include "main.h"
#include "common.h"
#include <stdbool.h>

#define TOUCH_SPI_PORT		hspi1
extern  SPI_HandleTypeDef 	TOUCH_SPI_PORT;

// If the following define enabled, decrease SPI speed when read touch screen data
// The touch screen controller is working fine on 1.3125 MBits/s
#define TOUCH_SPI_SPEED		(SPI_BAUDRATEPRESCALER_64)

#ifdef __cplusplus
extern "C" {
#endif

void	TOUCH_Init();
void	TOUCH_LoadCalibration(double cd[6]);
bool 	TOUCH_GetRawData(uint16_t* x, uint16_t* y);
bool	TOUCH_GetCoordinates(uint16_t* x, uint16_t* y);

#ifdef __cplusplus
}
#endif

#endif
