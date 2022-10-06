/*
 * low_level.h
 *
 *  Created on: Nov 20, 2020
 *      Author: Alex
 *
 * Low-level functions to deal with TFT displays
 *
 */
#ifndef _LOW_LEVEL_H
#define _LOW_LEVEL_H

#ifndef __cplusplus
#include <stdbool.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

void		TFT_SPI_DATA_MODE(void);
void		TFT_SPI_Command(uint8_t cmd, const uint8_t* buff, size_t buff_size);
void		TFT_SPI_SetAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
uint32_t	TFT_SPI_Read(uint8_t cmd, uint8_t length);
bool		TFT_SPI_ReadData(uint8_t cmd, uint8_t *data, uint16_t size);
void		TFT_SPI_ColorBlockInit(void);
void		TFT_ColorBlockSend(uint16_t color, uint32_t size);
void		TFT_SPI_ColorBlockSend_16bits(uint16_t color, uint32_t size);
void		TFT_SPI_ColorBlockSend_18bits(uint16_t color, uint32_t size);
void		TFT_SPI_ColorBlockFlush(void);

#ifdef __cplusplus
}
#endif

#endif
