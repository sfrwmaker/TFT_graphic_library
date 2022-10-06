/*
 * common.h
 *
 *  Created on: Nov 17, 2020
 *      Author: Alex
 *
 *  Base component of a library. Contains set of general functions to manage TFT display family.
 *  General approach is to set rectangular area on the screen and send array of colors to fill-up that area.
 *
 *  Based on the following libraries:
 *   martnak /STM32-ILI9341, 	https://github.com/martnak/STM32-ILI9341
 *   afiskon /stm32-ili9341,	https://github.com/afiskon/stm32-ili9341
 *   Bodmer /TFT_eSPI,			https://github.com/Bodmer/TFT_eSPI
 *   olikraus /u8glib,			https://github.com/olikraus/u8glib
 *
 *  Each particular display driver have to implement two native functions:
 *  <display>_Init(void) - to initialize the display
 *  <display>_SetRotation(tRotation rotation)
 *  because these methods depends on hardware.
 *
 *  CS & DC pins should share the same GPIO port
 */

#ifndef _COMMON_H
#define _COMMON_H

#include "main.h"
#include "low_level.h"
#include <stdbool.h>

#define TFT_SPI_PORT		hspi1
extern SPI_HandleTypeDef 	TFT_SPI_PORT;

// To disable DMA support, comment out next line
//#define TFT_USE_DMA			1

/* Include code for BMP and JPEG drawings, external FAT drive required */
 #define TFT_BMP_JPEG_ENABLE

typedef enum {
	TFT_ROTATION_0 	 = 0,
	TFT_ROTATION_90  = 1,
	TFT_ROTATION_180 = 2,
	TFT_ROTATION_270 = 3
} tRotation;

typedef enum {
	BLACK		= 0x0000,
	NAVY    	= 0x000F,
	DARKGREEN	= 0x03E0,
	DARKCYAN    = 0x03EF,
	MAROON		= 0x7800,
	PURPLE      = 0x780F,
	OLIVE       = 0x7BE0,
	LIGHTGREY	= 0xC618,
	DARKGREY	= 0x7BEF,
	BLUE		= 0x001F,
	GREEN       = 0x07E0,
	CYAN		= 0x07FF,
	RED			= 0xF800,
	MAGENTA		= 0xF81F,
	YELLOW		= 0xFFE0,
	WHITE		= 0xFFFF,
	ORANGE		= 0xFD20,
	GREENYELLOW	= 0xAFE5,
	PINK        = 0xF81F,
	GREY		= 0x52AA
} tColor;

typedef void 		(*t_SetAddrWindow)(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
typedef void 		(*t_StartDrawArea)(uint16_t x0, uint16_t y0, uint16_t width, uint16_t height);
typedef void 		(*t_DrawFilledRect)(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color);
typedef void 		(*t_DrawPixel)(uint16_t x,  uint16_t y, uint16_t color);
typedef void 		(*t_ColorBlockSend)(uint16_t color, uint32_t size);
typedef uint16_t	(*t_NextPixel)(uint16_t row, uint16_t col);

typedef struct {
	t_StartDrawArea		pStartDrawArea;
	t_DrawFilledRect	pDrawFilledRect;
	t_DrawPixel			pDrawPixel;
} tTFT_INTFUNC;

typedef enum {
	TFT_16bits	= 0,
	TFT_18bits	= 1
} tTFT_PIXEL_BITS;

typedef double (*LineThickness)(uint16_t pos, uint16_t length);

#ifdef __cplusplus
extern "C" {
#endif

// Functions to draw graphic primitives
uint16_t	TFT_Width(void);
uint16_t	TFT_Height(void);
tRotation 	TFT_Rotation(void);
uint16_t	TFT_WheelColor(uint8_t wheel_pos);
uint16_t	TFT_Color(uint8_t red, uint8_t green, uint8_t blue);
void 		TFT_FillScreen(uint16_t color);
void		TFT_DrawFilledRect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color);
void		TFT_DrawPixel(uint16_t x,  uint16_t y, uint16_t color);
void		TFT_DrawHLine(uint16_t x, uint16_t y, uint16_t length, uint16_t color);
void		TFT_DrawVLine(uint16_t x, uint16_t y, uint16_t length, uint16_t color);
void		TFT_DrawRect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color);
void		TFT_DrawRoundRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t r, uint16_t color);
void		TFT_DrawFilledRoundRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t r, uint16_t color);
void		TFT_DrawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color);
void		TFT_DrawCircle(uint16_t x, uint16_t y, uint8_t radius, uint16_t color);
void		TFT_DrawFilledCircle(uint16_t x, uint16_t y, uint8_t radius, uint16_t color);
void		TFT_DrawTriangle(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);
void		TFT_DrawfilledTriangle (uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);
void		TFT_DrawEllipse(uint16_t x0, uint16_t y0, uint16_t rx, uint16_t ry, uint16_t color);
void		TFT_DrawFilledEllipse(uint16_t x0, uint16_t y0, uint16_t rx, uint16_t ry, uint16_t color);
void		TFT_DrawArea(uint16_t x0, uint16_t y0, uint16_t area_width, uint16_t area_height, t_NextPixel nextPixelCB);
void		TFT_DrawBitmap(uint16_t x0, uint16_t y0, uint16_t area_width, uint16_t area_height,
				const uint8_t *bitmap, uint16_t bm_width, uint16_t bg_color, uint16_t fg_color);
void		TFT_DrawScrolledBitmap(uint16_t x0, uint16_t y0, uint16_t area_width, uint16_t area_height,
				const uint8_t *bitmap, uint16_t bm_width, int16_t offset, uint8_t gap, uint16_t bg_color, uint16_t fg_color);
void 		TFT_DrawPixmap(uint16_t x0, uint16_t y0, uint16_t area_width, uint16_t area_height,
				const uint8_t *pixmap, uint16_t pm_width, uint8_t depth, uint16_t palette[]);
void		TFT_DrawThickLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint8_t thickness, uint16_t color);
void		TFT_DrawVarThickLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, LineThickness thickness, uint16_t color);

// Functions to manage display
void		TFT_DEF_Reset(void);
void		TFT_DEF_SleepIn(void);
void		TFT_DEF_SleepOut(void);
void		TFT_DEF_InvertDisplay(bool on);
void		TFT_DEF_DisplayOn(bool on);
void		TFT_DEF_IdleMode(bool on);
uint16_t 	TFT_DEF_ReadPixel(uint16_t x, uint16_t y, bool is16bit_color);

// Functions to setup display
void		TFT_Setup(uint16_t generic_width, uint16_t generic_height, uint8_t madctl[4], tTFT_INTFUNC *pINT);
void		TFT_Pixel_Setup(tTFT_PIXEL_BITS pixel_bits);
void		TFT_SetRotation(tRotation rotation);

// Interface to extend functionality
void		TFT_SetAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
void		TFT_StartDrawArea(uint16_t x0, uint16_t y0, uint16_t width, uint16_t height);
void		TFT_FinishDrawArea();

// Low-level functions
void		TFT_SPI_DATA_MODE(void);

#ifdef __cplusplus
}
#endif

#define		TFT_Delay(a)	HAL_Delay(a);

#endif
