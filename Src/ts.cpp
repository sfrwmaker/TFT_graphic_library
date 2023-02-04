/*
 * ts.cpp
 *
 *  Created on: Nov 16, 2022
 *      Author: Alex
 */

#include <stdio.h>
#include "ts.h"
#include "calibration.h"

#ifdef WITH_TOUCH

extern tft dspl;

static void DrawCross(uint16_t x, uint16_t y, uint16_t color) {
	uint16_t sc_height 	= dspl.height();
	uint16_t sc_width 	= dspl.width();
	uint16_t h_x = (x > 5)?(x-5):0;
	uint16_t h_r = (x < sc_width-5)?(x+5):sc_width;
	uint16_t v_y = (y > 5)?(y-5):0;
	uint16_t v_b = (y < sc_height-5)?(y+5):sc_height;
	dspl.drawHLine(h_x, y, h_r-h_x+1, color);
	dspl.drawVLine(x, v_y, v_b-v_y+1, color);
}

void Touch(tRotation rotation) {
	dspl.setRotation(rotation);
	dspl.fillScreen(WHITE);
	dspl.setFont(u8g2_font_profont22_tf);
	dspl.drawStr(10, 100, "touch", BLACK);
	uint16_t sc_height 	= dspl.height();
	uint8_t  fnt_height	= dspl.getMaxCharHeight();
	char buff[20];
	uint32_t finish = HAL_GetTick() + 30000;
	while (HAL_GetTick() < finish) {
		uint16_t x = 0;
		uint16_t y = 0;
		if (TOUCH_GetCoordinates(&x, &y)) {
			sprintf(buff, "(%d, %d)", x, y);
			uint16_t w = dspl.drawStr(0, sc_height-1, buff, BLACK);
			HAL_Delay(500);
			dspl.drawFilledRect(0, sc_height-fnt_height, w, fnt_height+1, WHITE);
			DrawCross(x, y, RED);
		}
		HAL_Delay(20);
	}
}

void TouchCalibrate(void) {
	dspl.setRotation(TFT_ROTATION_0);
	dspl.fillScreen(WHITE);
	dspl.setFont(u8g2_font_profont22_tf);
	dspl.drawStr(10, 100, "touch calibration", BLACK);
	uint16_t sc_height 	= dspl.height();
	uint16_t sc_width 	= dspl.width();
	uint8_t  fnt_height	= dspl.getMaxCharHeight();
	char buff[20];
	CALB_Init();

	// Draw the calibration points one by one and read the touch screen data on them
	tPOINT cp[9];						// Calibration points
	cp[0].x	= 8;			cp[0].y = 8;
	cp[1].x = sc_width/2;	cp[1].y = 8;
	cp[2].x = sc_width-8;	cp[2].y	= 8;
	cp[3].x = 8;			cp[3].y = sc_height/2;
	cp[4].x = sc_width/2;	cp[4].y = sc_height/2;
	cp[5].x = sc_width-8;	cp[5].y = sc_height/2;
	cp[6].x	= 8;			cp[6].y = sc_height-8;
	cp[7].x = sc_width/2;	cp[7].y = sc_height-8;
	cp[8].x = sc_width-8;	cp[8].y	= sc_height-8;
	for (uint8_t dot = 0; dot < 9; ++dot) {
		uint16_t x, y;
		DrawCross(cp[dot].x, cp[dot].y, RED);
		while (!TOUCH_GetRawData(&x, &y));
		if (!CALB_add(x, y, cp[dot].x, cp[dot].y)) {
			break;
		}
		sprintf(buff, "(%d, %d)", x, y);
		uint16_t w = dspl.drawStr(0, sc_height-1, buff, BLACK);
		HAL_Delay(500);
		dspl.drawFilledRect(0, sc_height-fnt_height, w, fnt_height+1, WHITE);
		DrawCross(cp[dot].x, cp[dot].y, WHITE);
	}
	// Calculate touch screen calibration data
	double calib_data[6];
	if (CALB_calculate(calib_data)) {
		TOUCH_LoadCalibration(calib_data);
	}
}

#endif 		// WITH_TOUCH
