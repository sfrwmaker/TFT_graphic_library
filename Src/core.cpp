/*
 * core.cpp
 *
 *
 */

#include <math.h>
#include <algorithm>
#include <stdio.h>
#include "core.h"
#include "graph.h"
#include "tft.h"
#include "ff.h"

#ifdef WITH_TOUCH
#include "ts.h"
#endif

static const uint8_t bmArrowH[] = {
  0b11100000,
  0b00111100,
  0b11111111,
  0b00111100,
  0b11100000
};

static const uint8_t bmArrowV[] = {
  0b00100000,
  0b00100000,
  0b01110000,
  0b01110000,
  0b01110000,
  0b11111000,
  0b10101000,
  0b10101000
};

static const uint8_t bmFan[4][288] = {
  {	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
	0X00,0X00,0X00,0X00,0X7F,0X80,0X00,0X00,0X00,0X00,0XFF,0XC0,0X00,0X00,0X00,0X00,
	0XFF,0XC0,0X00,0X00,0X00,0X00,0X7F,0XC0,0X00,0X00,0X00,0X00,0X3F,0XC0,0X00,0X00,
	0X00,0X00,0X3F,0XC0,0X00,0X00,0X00,0X00,0X1F,0XC0,0X00,0X00,0X00,0X00,0X1F,0XC0,
	0X00,0X00,0X00,0X00,0X0F,0XC0,0X00,0X00,0X00,0X00,0X0F,0XC0,0X00,0X00,0X00,0X00,
	0X07,0XC0,0X00,0X00,0X00,0X00,0X07,0XC0,0X00,0X00,0X00,0X00,0X07,0XC0,0X00,0X00,
	0X00,0X00,0X03,0XC0,0X00,0X00,0X00,0X00,0X03,0XC0,0X00,0X00,0X00,0X00,0X03,0XC0,
	0X00,0X00,0X00,0X00,0X03,0XC0,0X00,0X00,0X00,0X00,0X0F,0XF0,0X00,0X00,0X00,0X00,
	0X0F,0XF0,0X00,0X00,0X00,0X00,0X1F,0XF8,0X00,0X00,0X00,0X00,0X1F,0XF8,0X00,0X00,
	0X00,0X00,0X1F,0XF8,0X00,0X00,0X00,0X00,0X3F,0XFC,0X00,0X00,0X00,0X00,0XEF,0XF6,
	0X00,0X00,0X00,0X03,0XFF,0XEF,0X80,0X00,0X00,0X07,0XFB,0XDF,0XF0,0X00,0X00,0X1F,
	0XF0,0X1F,0XFF,0XF8,0X00,0X7F,0XE0,0X07,0XFF,0XF8,0X00,0XFF,0XC0,0X03,0XFF,0XF8,
	0X03,0XFF,0X80,0X01,0XFF,0XF0,0X03,0XFF,0X00,0X00,0XFF,0XF0,0X03,0XFF,0X00,0X00,
	0X7F,0XE0,0X03,0XFE,0X00,0X00,0X1F,0XC0,0X01,0XFC,0X00,0X00,0X0F,0X80,0X01,0XFC,
	0X00,0X00,0X07,0X00,0X00,0XF8,0X00,0X00,0X00,0X00,0X00,0XF8,0X00,0X00,0X00,0X00,
	0X00,0X78,0X00,0X00,0X00,0X00,0X00,0X10,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00 },
  { 0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
	0X00,0X00,0X00,0X00,0X00,0X07,0X00,0X00,0X00,0X00,0X00,0X07,0XC0,0X00,0X00,0X00,
	0X00,0X07,0XE0,0X00,0X00,0X00,0X00,0X07,0XF0,0X00,0X00,0X00,0X00,0X07,0XF8,0X00,
	0X00,0X00,0X00,0X07,0XFC,0X00,0X00,0X00,0X00,0X07,0XFC,0X00,0X00,0X00,0X00,0X07,
	0XFC,0X00,0X00,0X00,0X00,0X07,0XF8,0X00,0X00,0X00,0X00,0X0F,0XF0,0X00,0X00,0X00,
	0X00,0X0F,0XE0,0X00,0X00,0X00,0X00,0X0F,0XE0,0X00,0X00,0X00,0X00,0X1F,0XC0,0X00,
	0X00,0X00,0X00,0X1F,0X80,0X00,0X00,0X00,0X00,0X3F,0X00,0X00,0X00,0X00,0X00,0X7E,
	0X00,0X00,0X00,0X00,0X03,0XDC,0X00,0X00,0X00,0X00,0X0F,0XEC,0X00,0X00,0X00,0X00,
	0X0F,0XF0,0X00,0X00,0X0F,0XFF,0XFF,0XF8,0X00,0X00,0X1F,0XFF,0XFF,0XF8,0X00,0X00,
	0X1F,0XFF,0XFF,0XF8,0X00,0X00,0X1F,0XFF,0XFF,0XF8,0X00,0X00,0X1F,0XFF,0X0F,0XF0,
	0X00,0X00,0X1F,0XF8,0X0F,0XF8,0X00,0X00,0X1F,0XE0,0X03,0XDC,0X00,0X00,0X1F,0X80,
	0X00,0X7E,0X00,0X00,0X1E,0X00,0X00,0X3F,0X00,0X00,0X0C,0X00,0X00,0X3F,0X80,0X00,
	0X00,0X00,0X00,0X1F,0XE0,0X00,0X00,0X00,0X00,0X1F,0XF0,0X00,0X00,0X00,0X00,0X0F,
	0XFC,0X00,0X00,0X00,0X00,0X07,0XFF,0X80,0X00,0X00,0X00,0X07,0XFF,0XC0,0X00,0X00,
	0X00,0X03,0XFF,0X80,0X00,0X00,0X00,0X03,0XFF,0X80,0X00,0X00,0X00,0X01,0XFF,0X00,
	0X00,0X00,0X00,0X00,0XFC,0X00,0X00,0X00,0X00,0X00,0XF0,0X00,0X00,0X00,0X00,0X00,
	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00 },
  {	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X03,0X80,0X00,0X00,0X00,0X00,
	0X07,0XC0,0X00,0X00,0X00,0X00,0X07,0XC0,0X03,0XC0,0X00,0X00,0X0F,0XC0,0X03,0XE0,
	0X00,0X00,0X0F,0XE0,0X07,0XF8,0X00,0X00,0X1F,0XE0,0X0F,0XFE,0X00,0X00,0X3F,0XE0,
	0X0F,0XFF,0X00,0X00,0X7F,0XF0,0X1F,0XFF,0XC0,0X00,0XFF,0XE0,0X1F,0XFF,0XF0,0X03,
	0XFF,0XE0,0X1F,0XFF,0XFF,0XEF,0XFF,0X80,0X3F,0XFF,0XFF,0XFF,0XFE,0X00,0X18,0X07,
	0XFF,0XF7,0XF0,0X00,0X00,0X00,0X7F,0XFF,0XC0,0X00,0X00,0X00,0X1F,0XFE,0X00,0X00,
	0X00,0X00,0X1F,0XF8,0X00,0X00,0X00,0X00,0X1F,0XF8,0X00,0X00,0X00,0X00,0X0F,0XF0,
	0X00,0X00,0X00,0X00,0X07,0XE0,0X00,0X00,0X00,0X00,0X03,0X80,0X00,0X00,0X00,0X00,
	0X03,0XE0,0X00,0X00,0X00,0X00,0X03,0XE0,0X00,0X00,0X00,0X00,0X03,0XE0,0X00,0X00,
	0X00,0X00,0X03,0XE0,0X00,0X00,0X00,0X00,0X03,0XF0,0X00,0X00,0X00,0X00,0X03,0XF0,
	0X00,0X00,0X00,0X00,0X03,0XF8,0X00,0X00,0X00,0X00,0X03,0XF8,0X00,0X00,0X00,0X00,
	0X03,0XFC,0X00,0X00,0X00,0X00,0X03,0XFC,0X00,0X00,0X00,0X00,0X03,0XFE,0X00,0X00,
	0X00,0X00,0X03,0XFF,0X00,0X00,0X00,0X00,0X03,0XFF,0X00,0X00,0X00,0X00,0X01,0XFF,
	0XC0,0X00,0X00,0X00,0X01,0XFF,0X80,0X00,0X00,0X00,0X00,0XFF,0X00,0X00,0X00,0X00,
	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00 },
  { 0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
	0X00,0X00,0X00,0X00,0X00,0X0F,0X80,0X00,0X00,0X00,0X00,0X3F,0XC0,0X00,0X00,0X00,
	0X00,0X7F,0XC0,0X00,0X00,0X00,0X00,0XFF,0XE0,0X00,0X00,0X00,0X01,0XFF,0XE0,0X00,
	0X00,0X00,0X01,0XFF,0XF0,0X00,0X00,0X00,0X00,0X7F,0XF0,0X00,0X00,0X00,0X00,0X1F,
	0XF8,0X00,0X00,0X00,0X00,0X07,0XFC,0X00,0X00,0X00,0X00,0X01,0XFC,0X00,0X00,0X00,
	0X00,0X00,0XFE,0X00,0X00,0X00,0X00,0X00,0X7E,0X00,0X00,0X00,0X00,0X00,0X3E,0X00,
	0X00,0X18,0X00,0X00,0X1F,0XE0,0X00,0X3C,0X00,0X00,0X1F,0XF0,0X00,0XFC,0X00,0X00,
	0X0F,0XF0,0X07,0XFC,0X00,0X00,0X1F,0XFC,0X7F,0XFC,0X00,0X00,0X1F,0XFF,0XFF,0XFC,
	0X00,0X00,0X1F,0XFF,0XFF,0XFC,0X00,0X00,0X1F,0XFF,0XFF,0XF8,0X00,0X00,0X0F,0XF7,
	0XFF,0XF8,0X00,0X00,0X0F,0XE0,0XFF,0XF0,0X00,0X00,0X1F,0XC0,0X03,0XF0,0X00,0X00,
	0X3F,0X00,0X00,0X00,0X00,0X00,0X3E,0X00,0X00,0X00,0X00,0X00,0X3E,0X00,0X00,0X00,
	0X00,0X00,0X7E,0X00,0X00,0X00,0X00,0X00,0X7E,0X00,0X00,0X00,0X00,0X00,0XFC,0X00,
	0X00,0X00,0X00,0X01,0XFC,0X00,0X00,0X00,0X00,0X01,0XFC,0X00,0X00,0X00,0X00,0X03,
	0XFC,0X00,0X00,0X00,0X00,0X03,0XFC,0X00,0X00,0X00,0X00,0X07,0XFC,0X00,0X00,0X00,
	0X00,0X07,0XFC,0X00,0X00,0X00,0X00,0X07,0XFC,0X00,0X00,0X00,0X00,0X03,0XFE,0X00,
	0X00,0X00,0X00,0X01,0XFE,0X00,0X00,0X00,0X00,0X00,0X7E,0X00,0X00,0X00,0X00,0X00,
	0X1C,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00 },
};

tft_ILI9488	dspl;									// Can be used by touch screen utilities (see ts.h)
static GRAPH		graph;
static PIXMAP		pixmap;
static BITMAP		bm_preset;
static FATFS		fs;

extern "C" void setup(void) {
	dspl.init();
	f_mount(&fs, "0:/", 1);
#ifdef WITH_TOUCH
	TouchCalibrate();
#endif
	dspl.fillScreen(CYAN);
	dspl.setRotation(TFT_ROTATION_180);				// Setup landscape mode
}

uint32_t Random(uint32_t min, uint32_t max) {
	if (min < max) {
		return rand() % (max-min) + min;
	}
	return min;
}

void fillScreen(void) {
	uint32_t ms = HAL_GetTick();
	uint16_t fills = 100;
	for (uint32_t i = 0; i < fills; ++i) {
		uint16_t c = dspl.wheelColor(Random(0, 255));
		dspl.fillScreen(c);
	}
	dspl.fillScreen(WHITE);
	ms = HAL_GetTick() - ms;
	dspl.fillScreen(CYAN);
	char buff[30];
	sprintf(buff, "%ld ms per %d fills", ms, fills);
	dspl.drawStr(10, 100, buff, BLACK);
	HAL_Delay(5000);
}

void hLines(uint16_t sc_width, uint16_t sc_height) {
	dspl.drawStr(10, 100, "horizontal lines", BLACK);
	HAL_Delay(2000);
	for (uint16_t y = 0; y < sc_height; ++y) {
		uint16_t x = y/4;
		uint16_t l = sc_width-y/2;
		uint16_t c = dspl.wheelColor(Random(0, 255));
		dspl.drawHLine(x, y, l, c);
	}
	HAL_Delay(5000);
	dspl.fillScreen(WHITE);
}

void vLines(uint16_t sc_width, uint16_t sc_height) {
	dspl.drawStr(10, 100, "vertical lines", BLACK); HAL_Delay(2000);
	for (uint16_t x = 0; x < sc_width; ++x) {
		uint16_t y = x/4;
		uint16_t l = sc_height-x/2;
		uint16_t c = dspl.wheelColor(Random(0, 255));
		dspl.drawVLine(x, y, l, c);
	}
	HAL_Delay(5000);
	dspl.fillScreen(WHITE);
}

void drawRects(uint16_t sc_width, uint16_t sc_height) {
	dspl.drawStr(10, 100, "rectangles", BLACK); HAL_Delay(2000);
	for (uint16_t x = 2; x < sc_width/2-4; ++x) {
		uint16_t w = sc_width  - 2 * x;
		uint16_t h = sc_height - 2 * x;
		uint16_t c = dspl.wheelColor(Random(0, 255));
		dspl.drawRect(x, x, w, h, c);
	}
	HAL_Delay(5000);
	dspl.fillScreen(WHITE);
}

void fillRects(uint16_t sc_width, uint16_t sc_height) {
	dspl.drawStr(10, 100, "fill area", BLACK); HAL_Delay(2000);
	uint16_t size = std::min(sc_width, sc_height);
	for (uint16_t x = 2; x < size/2; ++x) {
		uint16_t w = sc_width  - 2 * x;
		uint16_t h = sc_height - 2 * x;
		uint16_t c = dspl.wheelColor(Random(0, 255));
		dspl.drawFilledRect(x, x, w, h, c);
	}
	HAL_Delay(5000);
	dspl.fillScreen(WHITE);
}

void drawLines(uint16_t sc_width, uint16_t sc_height) {
	dspl.drawStr(10, 100, "lines", BLACK); HAL_Delay(2000);
	for (uint32_t x1 = 0; x1 < sc_width; ++x1) {
		uint16_t c = dspl.wheelColor(Random(0, 255));
		dspl.drawLine(0, 0, x1, sc_height-1, c);
	}
	for (uint16_t x1 = 0; x1 < sc_width; ++x1) {
		uint16_t c = dspl.wheelColor(Random(0, 255));
		dspl.drawLine(sc_width-1, sc_height-1, x1, 0, c);
	}
	for (uint16_t y1 = 0; y1 < sc_height; ++y1) {
		uint16_t c = dspl.wheelColor(Random(0, 255));
		dspl.drawLine(sc_width-1, 0, 0, y1, c);
	}
	for (uint16_t y1 = 0; y1 < sc_height; ++y1) {
		uint16_t c = dspl.wheelColor(Random(0, 255));
		dspl.drawLine(0, sc_height-1, sc_width-1, y1, c);
	}
	HAL_Delay(5000);
	dspl.fillScreen(WHITE);
}

void drawThickLines(uint16_t sc_width, uint16_t sc_height, uint32_t iterations) {
	dspl.drawStr(10, 100, "thick lines", BLACK); HAL_Delay(2000);
	for (uint32_t i = 0; i < iterations; ++i) {
		uint16_t x0 = Random(0, sc_width-8);
		uint16_t y0 = Random(0, sc_height-8);
		uint16_t x1 = Random(8, sc_width);
		uint16_t y1 = Random(8, sc_height);
		uint8_t  t  = Random(2, 10);
		uint16_t c = dspl.wheelColor(Random(0, 255));
		if (x0+x1 > sc_width)  x1 = sc_width - x0;
		if (y0+y1 > sc_height) y1 = sc_height - y0;
		dspl.drawThickLine(x0, y0, x1, y1, t, c);
	}
	dspl.fillScreen(WHITE);
}

void drawCircles(uint16_t sc_width, uint16_t sc_height, uint32_t iterations) {
	dspl.drawStr(10, 100, "cirles", BLACK); HAL_Delay(2000);
	for (uint32_t i = 0; i < iterations; ++i) {
		uint16_t x = Random(0, sc_width-8);
		uint16_t y = Random(0, sc_height-8);
		uint16_t r = Random(8, sc_width);
		uint16_t c = dspl.wheelColor(Random(0, 255));
		if (x < r) x = r;
		if (y < r) y = r;
		if (x+r > sc_width)  r = sc_width - x;
		if (y+r > sc_height) r = sc_height - y;
		dspl.drawCircle(x, y, r, c);
	}
	dspl.fillScreen(WHITE);
}

void drawDiscs(uint16_t sc_width, uint16_t sc_height, uint32_t iterations) {
	dspl.drawStr(10, 100, "discs", BLACK); HAL_Delay(2000);
	for (uint32_t i = 0; i < iterations; ++i) {
		uint16_t x = Random(0, sc_width-8);
		uint16_t y = Random(0, sc_height-8);
		uint16_t r = Random(8, sc_width/2);
		uint16_t c = dspl.wheelColor(Random(0, 255));
		if (x < r) x = r;
		if (y < r) y = r;
		if (x+r > sc_width)  r = sc_width - x;
		if (y+r > sc_height) r = sc_height - y;
		dspl.drawFilledCircle(x, y, r, c);
	}
	dspl.fillScreen(WHITE);
}

void drawTriangles(uint16_t sc_width, uint16_t sc_height) {
	dspl.drawStr(10, 100, "triangles", BLACK); HAL_Delay(2000);
	for (uint16_t y0 = 0; y0 < sc_height; ++y0) {
		uint16_t c = dspl.wheelColor(Random(0, 255));
		dspl.drawTriangle(sc_width/2, y0, 0, sc_height-1, sc_width-1, sc_height-1, c);
	}
	for (uint16_t y0 = 0; y0 < sc_height; ++y0) {
		uint16_t c = dspl.wheelColor(Random(0, 255));
		dspl.drawTriangle(sc_width/2, y0, 0, 0, sc_width-1, 0, c);
	}
	HAL_Delay(5000);
	dspl.fillScreen(WHITE);
}


void fillTriangles(uint16_t sc_width, uint16_t sc_height, uint32_t iterations) {
	dspl.drawStr(10, 100, "fill triangles", BLACK); HAL_Delay(2000);
	for (uint32_t i = 0; i < iterations; ++i) {
		uint16_t x0 = Random(0, sc_width);
		uint16_t y0 = Random(0, sc_height);
		uint16_t x1 = Random(0, sc_width);
		uint16_t y1 = Random(0, sc_height);
		uint16_t x2 = Random(0, sc_width);
		uint16_t y2 = Random(0, sc_height);
		uint16_t c = dspl.wheelColor(Random(0, 255));
		dspl.drawFilledTriangle(x0, y0, x1, y1, x2, y2, c);
	}
	dspl.fillScreen(WHITE);
}

void drawRoundRects(uint16_t sc_width, uint16_t sc_height, uint32_t iterations) {
	dspl.drawStr(10, 100, "round rectangles", BLACK); HAL_Delay(2000);
	uint16_t size = std::min(sc_height, sc_width);
	for (uint16_t x = 2; x < size/2-10; x+= 2) {
		uint16_t w = sc_width  - 2 * x;
		uint16_t h = sc_height - 2 * x;
		uint8_t  r = Random(2, 10);
		uint16_t c = dspl.wheelColor(Random(0, 255));
		dspl.drawRoundRect(x, x, w, h, r, c);
	}
	HAL_Delay(5000);
	dspl.fillScreen(WHITE);
}

void fillRoundRects(uint16_t sc_width, uint16_t sc_height, uint32_t iterations) {
	dspl.drawStr(10, 100, "fill round rectangles", BLACK); HAL_Delay(2000);
	for (uint32_t i = 0; i < iterations; ++i) {
		uint16_t x = Random(0, sc_width-8);
		uint16_t y = Random(0, sc_height-8);
		uint16_t w = Random(8, sc_width);
		uint16_t h = Random(8, sc_height);
		uint16_t r = w/4; if (r > h/4) r = h/4;
		if (r < 2) r = 2;
		r		   = Random(2, r);
		uint16_t c = dspl.wheelColor(Random(0, 255));
		if (x+w > sc_width)  w = sc_width - x;
		if (y+h > sc_height) h = sc_height - y;
		dspl.drawFilledRoundRect(x, y, w, h, r, c);
	}
	dspl.fillScreen(WHITE);
}

double lineThickness(uint16_t pos, uint16_t length) {
	if (pos < length/2) {
		return 1.0 + 10.0*fabs(sin((pos * M_PI * 2)/ length));
	}
	pos = length - pos;
	return 1.0 + sqrt(pos)/2.0;
}

void drawVarThickLines(uint16_t sc_width, uint16_t sc_height) {
	dspl.fillScreen(WHITE);
	dspl.drawStr(10, 100, "var. thick lines", BLACK); HAL_Delay(2000);
	uint16_t c = dspl.wheelColor(Random(0, 255));
	for (uint16_t x = 0; x < sc_width; x += 4) {
		dspl.drawVarThickLine(sc_width/2, sc_height/2, x, 0, lineThickness, c);
		HAL_Delay(10);
		dspl.drawVarThickLine(sc_width/2, sc_height/2, x, 0, lineThickness, WHITE);
	}
	c = dspl.wheelColor(Random(0, 255));
	for (uint16_t y = 0; y < sc_height; y += 4) {
		dspl.drawVarThickLine(sc_width/2, sc_height/2, sc_width-1, y, lineThickness, c);
		HAL_Delay(10);
		dspl.drawVarThickLine(sc_width/2, sc_height/2, sc_width-1, y, lineThickness, WHITE);
	}
	c = dspl.wheelColor(Random(0, 255));
	for (int16_t x = sc_width-1; x >= 0; x -= 4) {
		dspl.drawVarThickLine(sc_width/2, sc_height/2, x, sc_height-1, lineThickness, c);
		HAL_Delay(10);
		dspl.drawVarThickLine(sc_width/2, sc_height/2, x, sc_height-1, lineThickness,  WHITE);
	}
	c = dspl.wheelColor(Random(0, 255));
	for (int16_t y = sc_height-1; y >= 0; y -= 4) {
		dspl.drawVarThickLine(sc_width/2, sc_height/2, 0, y, lineThickness, c);
		HAL_Delay(10);
		dspl.drawVarThickLine(sc_width/2, sc_height/2, 0, y, lineThickness, WHITE);
	}
	dspl.fillScreen(WHITE);
}

void drawText(void) {
	dspl.fillScreen(WHITE);
	dspl.setFont(u8g2_font_crox5t_tf);
	uint16_t h = dspl.getMaxCharHeight();
	for (uint8_t p = 0; p < 2; ++p) {
		for (uint8_t i = 2; i < 8; ++i) {
			char buff[17];
			for (uint8_t j = 0; j < 16; ++j)
				buff[j] = (i*16+j) | (p << 7);
			buff[16] = '\0';
			uint16_t c = dspl.wheelColor(Random(0, 255));
			dspl.drawStr(10, (i-2)*h + p*6*h, buff, c);
		}
	}
	HAL_Delay(10000);
}

void drawScalledString(const char *str) {
	dspl.fillScreen(WHITE);
	dspl.setFont(u8g2_font_crox5t_tf);
	uint16_t y = 0;
	for (uint8_t s = 1; s <= 4; ++s) {
		dspl.setFontScale(s);
		uint16_t h = dspl.getMaxCharHeight();
		uint16_t c = dspl.wheelColor(Random(0, 255));
		y += h+5;
		dspl.drawStr(10, y, str, c);
	}
	dspl.setFontScale(1);
	HAL_Delay(10000);
}

void scrollString(void) {
	dspl.setFont(u8g2_font_inr19_mf);
	const char* str = "Long scrolling line";
	uint16_t h	= dspl.getMaxCharHeight();
	uint16_t w	= dspl.getStrWidth(str);
	BITMAP bm(w, h);
	if (bm.width() == 0) return;
	dspl.strToBitmap(bm, str, align_left, 0, false);
	for (uint8_t i = 0; i < 2; ++i) {
		uint8_t r = Random(0, 255);
		uint16_t bg = dspl.wheelColor(r);
		uint16_t fg = dspl.wheelColor(r+128);
		dspl.fillScreen(fg);
		uint8_t gap = 30;
		for (int16_t j = -220; j < w+gap; ++j) {
			dspl.drawScrolledBitmap(10, 50, 200, bm, j,    gap, bg, fg);
			dspl.drawScrolledBitmap(10, 100, 200, bm, j+10, gap, bg, fg);
			HAL_Delay(10);
		}
		for (int16_t j = w+gap-1; j >= -220; --j) {
			dspl.drawScrolledBitmap(10, 50, 200, bm, j,    gap, fg, bg);
			dspl.drawScrolledBitmap(10, 100, 200, bm, j+10, gap, fg, bg);
			HAL_Delay(10);
		}
	}
}

void animateFan(uint16_t sc_width, uint16_t sc_height, uint32_t iterations) {
	dspl.fillScreen(WHITE);
	dspl.drawStr(10, 50, "animate fan icon", BLACK);
	uint8_t r = Random(0, 255);
	uint8_t fan_angle = 0;
	for (uint32_t i = 0; i < iterations; ++i) {
		uint16_t fan_clr = dspl.wheelColor(r);
		dspl.drawIcon((sc_width >> 1) - 24, (sc_height >> 1) - 24, 48, 48, bmFan[fan_angle], 48, WHITE, fan_clr);
		++fan_angle;
		fan_angle &= 0x3;										// Can be from 0 to 3
		++r;
		HAL_Delay(50);
	}
}

// Draw three digits value 0-999
void drawValue(uint16_t value, uint16_t x, uint16_t y, BM_ALIGN align, uint16_t color) {
	if (value > 999) value = 999;
	char b[6];
	sprintf(b, "%d", value);
	dspl.setFont(u8g_font_profont22r);
	bm_preset.clear();
	dspl.strToBitmap(bm_preset, b, align);
	dspl.drawBitmap(x, y, bm_preset, BLACK, color);
}

void animateGraph(uint16_t sc_width, uint16_t sc_height, uint32_t iterations) {
	const uint16_t fg_color = 0xFFFF;
	const uint16_t bg_color = 0;
	const uint16_t gd_color = RED;
	const uint16_t dp_color = BLUE;
	const char *title = "graph";
	const char *temp  = "T";
	const char *disp  = "D";

	// Allocate Bitmap for preset temperature (3 symbols)
	dspl.setFont(u8g_font_profont22r);
	uint16_t h	= dspl.getMaxCharHeight();
	uint16_t w	= dspl.getStrWidth("000") + 2;
	bm_preset	= BITMAP(w, h);

	dspl.fillScreen(WHITE);
	dspl.drawStr(10, 50, "color graph", bg_color); HAL_Delay(2000);
	dspl.fillScreen(BLACK);
	h += 5;													// Extra space between lines
	uint16_t top = h+30;
	uint16_t t_height = sc_height-top-5;					// The graph height, leave 5 bottom lines free
	if ((t_height & 1) == 0) t_height--;					// Ensure the graph height is odd to draw abscissa coordinate axis

	uint16_t data_size = sc_width - bm_preset.width() - 54;
	if (graph.allocate(data_size)) {
		// Allocate space for graph pixmap
		if (pixmap.width() == 0) {
			pixmap = PIXMAP(data_size, t_height, 2);		// Depth is 2 bits, 4-color graph
			uint16_t g_colors[4] = { bg_color, fg_color, gd_color, dp_color};
			pixmap.setupPalette(g_colors, 4);
		}
	}

	// Check both bitmaps allocated successfully
	if (pixmap.width() == 0) return;
	t_height  				 = pixmap.height();				// The temperature graph height, leave 5 bottom lines free
	const uint8_t  temp_zero = t_height/2;					// The temperature abscissa axis vertical coordinate inside bitmap
    const uint8_t  disp_zero = t_height-1;					// The dispersion  abscissa axis vertical coordinate

	// Show the axis of coordinates
	const uint16_t ord = bm_preset.width()+19;				// The graph ordinate axis X coordinate
	dspl.drawHLine(bm_preset.width()+5, temp_zero+top, sc_width-70, fg_color); // The abscissa axis (time)
	dspl.drawIcon(sc_width-70+bm_preset.width()+5, temp_zero+top-2, 8, 5, bmArrowH, 8, bg_color, fg_color); // abscissa axis arrow
	dspl.drawStr(sc_width-20, temp_zero+top-5, "t", fg_color);	// The axis label, time
	dspl.drawVLine(ord, 10, sc_height-12, fg_color);			// The ordinate axis (temperature/dispersion)
	dspl.drawIcon(ord-2, 5, 5, 8, bmArrowV, 8, bg_color, fg_color);	// The ordinate axis arrow
	dspl.drawHLine(ord-4, top, 9, gd_color);						// Maximum temperature label
	dspl.drawHLine(ord-4, top+h, 9, dp_color);					// Maximum dispersion label
	// Show graph title and axis labels
	dspl.drawStr(bm_preset.width()-15, h, temp, gd_color);
	uint16_t hp = dspl.drawStr(bm_preset.width()+30, h, disp, dp_color) + bm_preset.width()+30;
	w = dspl.getStrWidth(title);
	dspl.drawStr(hp+(sc_width-hp-w)/2, h, title, fg_color);

    int16_t  t = 0;
    int16_t d = 0;
    for (uint32_t i = 0; i < iterations; ++i) {
    	t += Random(0, 10) - 5;
    	d += Random(0, 10) - 5;
    	if (d < 0 ) d = 0;
    	graph.put(t, d);
		// Calculate the transition coefficient for the temperature, dispersion and applied power
		int16_t	 min_t = 32767;
		int16_t  max_t = -32767;
		uint16_t max_d = 0;										// Maximum value for dispersion
		uint16_t till  = graph.dataSize();
		for (uint8_t i = 0; i < till; ++i) {
			int16_t  t = graph.temp(i);
			uint16_t d = graph.disp(i);
			if (min_t > t) min_t = t;							// Here h_temp is average_temp - preset_temp
			if (max_t < t) max_t = t;
			if (max_d < d) max_d = d;
		}
		if (min_t < 0)		min_t *= -1;						// If graph under zero is bigger (the temperature is lower than preset one)
		if (max_t < min_t)	max_t = min_t;						// normalize graph by its lower part
		uint16_t d_height = t_height - h;						// Dispersion graph height is lower because we should write max dispersion value

		// Normalize the graph data and fill-up the graph pixmap
		pixmap.clear();
		int16_t g0[2]	= {0};									// Previous value of graph: temperature, dispersion
		int16_t g1[2]	= {0};									// Current  value of graph: temperature, dispersion
		for (uint16_t i = 0; i < till-1; ++i) {					// fill-up the graph bitmaps
			// Normalize the data to be plotted. Current points are g1[x]
			g0[0] = g1[0];										// Temperature graph. Save previous data
			if (max_t == 0) {
				g1[0] = temp_zero;
			} else {
				int16_t t = graph.temp(i);
				if (t > 0) {
					g1[0] = temp_zero - round((float)t * (float)temp_zero / (float)max_t);
					if (g1[0] < 1) g1[0] = 1;
				} else {
					int16_t neg = t * (-1);
					g1[0] = temp_zero + round((float)neg * (float)temp_zero / (float)max_t);
					if (g1[0] >= t_height) g1[0] = t_height - 1;
				}
			}
			g0[1] = g1[1];										// Dispersion graph. Save previous data
			if (max_d == 0) {
				g1[1] = disp_zero;
			} else {
				int16_t d = round((float)graph.disp(i) * (float)d_height / (float)max_d);
				if (d >= d_height) d = d_height-1;
				g1[1] = disp_zero - d;
			}
			// draw line between nearby points from t0 to t1, from d0 to d1, from p0 and p1
			if (i > 0) {										// we need two points to draw the line
				for (int8_t gr = 1; gr >= 0; --gr) {			// Through the graphs
					uint16_t top_dot = g1[gr];					// draw vertical line from top_dot and length is len
					uint16_t len = 0;
					if (g1[gr] <= g0[gr]) {
						len = g0[gr] - g1[gr] + 1;
					} else {
						top_dot = g0[gr];
						len = g1[gr] - g0[gr] + 1;
					}
					pixmap.drawVLineCode(i-1, top_dot, len, gr+2);
				}
			}
		}
		pixmap.drawHLineCode(0, temp_zero, pixmap.width(), 1);	// Draw the temperature abscissa axis
		dspl.drawPixmap(bm_preset.width()+20, top, pixmap.width(), t_height, pixmap);

		// draw graph maximum value labels and applied power
		drawValue(max_t, 0, top-h/2, align_right, gd_color);	// Show maximum value of temperature
		drawValue(max_d, 0, top+h/2, align_right, dp_color);	// Show maximum value of dispersion
		HAL_Delay(200);
    }
	pixmap.~PIXMAP();
	graph.freeData();
	bm_preset.~BITMAP();
}

// UTF8 fonts have small size.
void rusUTF8(void) {
	dspl.fillScreen(WHITE);
	// Set UTF-8 encoding "Edit->set encoding"
	const char *ru_txt = "Русский UTF8";
	dspl.setFont(u8g2_font_10x20_t_cyrillic);
	dspl.drawUTF8(20, 50, ru_txt, BLUE);
	// Compare font size
	const char *txt = "Compare font size";
	dspl.setFont(u8g_font_profont22r);
	dspl.drawStr(20, 100, txt, BLUE);
	HAL_Delay(5000);
}

void chineeseHello(void) {
	dspl.fillScreen(WHITE);
	dspl.setFont(u8g2_font_unifont_t_chinese2);
	dspl.drawUTF8(20, 50, "你好世界", BLUE);
	HAL_Delay(5000);
}

void greekUTF8(void) {
	dspl.fillScreen(WHITE);
	dspl.setFont(u8g2_font_cu12_t_greek);
	dspl.drawUTF8(20, 50, "ΔΙΑΒΑΣΤΕ ΑΚΟΜΗ", BLUE);
	HAL_Delay(5000);
}

void drawBmpTS(const char *filename) {
	uint32_t ms = HAL_GetTick();
	dspl.drawBMP(filename, 0, 0);
	ms = HAL_GetTick() - ms;
	char buff[10];
	sprintf(buff, "%ld ms", ms);
	dspl.drawStr(10, 270, filename, WHITE);
	uint16_t lw = dspl.drawStr(10, 300, buff, WHITE);
	HAL_Delay(5000);
	ms = HAL_GetTick();
	dspl.clipBMP(filename, 0, 0, 10, 280, lw, 20);
	ms = HAL_GetTick() - ms;
	sprintf(buff, "%ld ms", ms);
	dspl.drawStr(10, 300, buff, WHITE);
	HAL_Delay(1000);
}

void drawJpgTS(const char *filename) {
	uint32_t ms = HAL_GetTick();
	dspl.drawJPEG(filename, 0, 0);
	ms = HAL_GetTick() - ms;
	char buff[10];
	sprintf(buff, "%ld ms", ms);
	dspl.drawStr(10, 270, filename, WHITE);
	uint16_t lw = dspl.drawStr(10, 300, buff, WHITE);
	HAL_Delay(5000);
	ms = HAL_GetTick();
	dspl.clipJPEG(filename, 0, 0, 10, 280, lw, 20);
	ms = HAL_GetTick() - ms;
	sprintf(buff, "%ld ms", ms);
	dspl.drawStr(10, 300, buff, WHITE);
	HAL_Delay(5000);
}

void drawPicture(void) {
	drawBmpTS("v1.bmp");
	drawBmpTS("v2.bmp");
	drawJpgTS("v3.jpg");
	drawBmpTS("v4.bmp");
}

void scrollStringOverBMP(const char *filename) {
	dspl.drawBMP(filename, 0, 0);
	dspl.setFont(u8g2_font_inr19_mf);
	const char* str = "Text over BMP image";
	uint16_t height	= dspl.getMaxCharHeight();
	uint16_t width	= dspl.getStrWidth(str);
	BITMAP bm(width, height);
	dspl.strToBitmap(bm, str);
	for (uint8_t i = 0; i < 3; ++i) {
		uint8_t r = Random(0, 255);
		uint16_t fg = dspl.wheelColor(r);
		uint8_t gap = 30;
		for (int16_t j = -220; j < width+gap; j += 4) {
			dspl.scrollBitmapOverBMP(filename, 0, 0, 10, 50, 200, height, bm, j, gap, fg);
			HAL_Delay(200);
		}
		for (int16_t j = width+gap; j >= -150; j -= 4) {
			dspl.scrollBitmapOverBMP(filename, 0, 0, 10, 50, 200, height, bm, j, gap, fg);
			HAL_Delay(50);
		}
	}
	dspl.jpegDeallocate();
}

extern "C" void loop(void) {
	dspl.fillScreen(CYAN);
	dspl.setFont(u8g2_font_profont22_tf);
	fillScreen();
	dspl.fillScreen(CYAN);
	uint16_t sc_width  	= dspl.width();
	uint16_t sc_height 	= dspl.height();
	const uint32_t n	= 300;
#ifdef WITH_TOUCH
	// Test Touch screen feature
	Touch(TFT_ROTATION_180);
	dspl.fillScreen(WHITE);
#endif
	hLines(sc_width, sc_height);
	vLines(sc_width, sc_height);
	drawRects(sc_width, sc_height);
	fillRects(sc_width, sc_height);
	drawLines(sc_width, sc_height);
	drawThickLines(sc_width, sc_height, n);
	drawCircles(sc_width, sc_height, n);
	drawDiscs(sc_width, sc_height, n);
	drawTriangles(sc_width, sc_height);
	fillTriangles(sc_width, sc_height, n);
	drawRoundRects(sc_width, sc_height, n);
	fillRoundRects(sc_width, sc_height, n);
	drawVarThickLines(sc_width, sc_height);
	animateFan(sc_width, sc_height, n);
	drawText();
	drawScalledString("scalled");
	rusUTF8();
	chineeseHello();
	greekUTF8();
	scrollString();
	animateGraph(sc_width, sc_height, n);

	if (fs.fs_type) {											// Filesystem type (0:not mounted)
		drawPicture();
		scrollStringOverBMP("v4.bmp");
	}
	HAL_Delay(10000);
}

