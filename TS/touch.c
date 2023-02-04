/*
 * touch.c
 *
 *  Created on: May 22 2020
 *      Author: Alex
 *
 * Procedures that manage SPI variant of XPT2046 touch screen
 */

#include "touch.h"
#include <math.h>

static const uint16_t XPT2046_MIN_RAW_X	= 1500;
static const uint16_t XPT2046_MAX_RAW_X	= 30000;
static const uint16_t XPT2046_MIN_RAW_Y	= 1800;
static const uint16_t XPT2046_MAX_RAW_Y = 30000;
static const uint16_t Z_THRESHOLD		= 300;

// (0, 0) coordinate of the touch screen is lower-left corner when display is not rotated
// (0, 0) coordinate of the display      is upper-left corner when display is not rotated
// So we must inverse Y coordinate (see calib_data[4] & calib_data[5]

// calibration data is the array of calibration coefficients
// calib_data[0] = A, calib_data[1] = B, ... calib_data[5] = F
// screen coordinates (x, y) can be calculated from touch coordinates (X, Y) as:
// x = A*X + B*Y + C
// y = D*X + E*Y + F
static double calib_data[6] = { 1, 0, 0, 0, 1, 0 };

void TOUCH_Init() {
    HAL_GPIO_WritePin(TOUCH_CS_GPIO_Port, TOUCH_CS_Pin, GPIO_PIN_SET);
    double sh = TFT_Height();
    double sw = TFT_Width();
    calib_data[0] = sw / ((double)XPT2046_MAX_RAW_X - (double)XPT2046_MIN_RAW_X);
    calib_data[1] = 0;
    calib_data[2] = (double)XPT2046_MIN_RAW_X * calib_data[0] * (-1.0);
    calib_data[4] = sh / ((double)XPT2046_MAX_RAW_Y - (double)XPT2046_MIN_RAW_Y) * (-1.0);
    calib_data[5] = (double)XPT2046_MIN_RAW_Y * calib_data[4] * (-1.0) + sh;
}

void TOUCH_LoadCalibration(double cd[6]) {
	for (uint8_t i = 0; i < 6; ++i)
		calib_data[i] = cd[i];
}

/*
 * The procedure to read the RAW coordinate of touched point
 * TOUCH_IRQ pin does not get low when the display touched at least when SPI bus is connected. Have no idea why.
 * To ensure the display was touched,
 */
bool TOUCH_GetRawData(uint16_t* x, uint16_t* y) {
	static const uint8_t cmd_read_Z1	= 0xB1;	// Keep powered
	static const uint8_t cmd_read_Z2	= 0xC1;
    static const uint8_t cmd_read_X		= 0xD1;
    static const uint8_t cmd_read_Y		= 0x91;
    static const uint8_t cmd_read_YL	= 0x90;	// Last read, power down
    static uint8_t data_tx[6]  	= { 0 };

/*
 * TOUCH_IRQ pin does not get low when the display touched at least when SPI bus is connected. Have no idea why.
    // Do not read the RAW data if not touched
    if (HAL_GPIO_RESET != HAL_GPIO_GetPin(TOUCH_IRQ_GPIO_Port, TOUCH_IRQ_Pin))
    	return false;
*/
    HAL_GPIO_WritePin(TOUCH_CS_GPIO_Port, TOUCH_CS_Pin, GPIO_PIN_RESET);

    // Decrease SPI speed to read the touch screen data
#ifdef TOUCH_SPI_SPEED
    uint32_t spi_speed = TOUCH_SPI_PORT.Init.BaudRatePrescaler;
    TOUCH_SPI_PORT.Init.Mode				= SPI_MODE_MASTER;
    TOUCH_SPI_PORT.Init.Direction			= SPI_DIRECTION_2LINES;
    TOUCH_SPI_PORT.Init.DataSize			= SPI_DATASIZE_8BIT;
    TOUCH_SPI_PORT.Init.CLKPolarity 		= SPI_POLARITY_LOW;
    TOUCH_SPI_PORT.Init.CLKPhase 			= SPI_PHASE_1EDGE;
    TOUCH_SPI_PORT.Init.NSS 				= SPI_NSS_SOFT;
    TOUCH_SPI_PORT.Init.BaudRatePrescaler	= TOUCH_SPI_SPEED;
    TOUCH_SPI_PORT.Init.FirstBit 			= SPI_FIRSTBIT_MSB;
    TOUCH_SPI_PORT.Init.TIMode 				= SPI_TIMODE_DISABLE;
    TOUCH_SPI_PORT.Init.CRCCalculation 		= SPI_CRCCALCULATION_DISABLE;
    TOUCH_SPI_PORT.Init.CRCPolynomial 		= 10;
    HAL_SPI_Init(&TOUCH_SPI_PORT);
#endif

    uint32_t avg_x = 0;
    uint32_t avg_y = 0;
    uint8_t data[6];
    data_tx[0] = cmd_read_Z1;
    data_tx[3] = cmd_read_Z2;
    HAL_SPI_TransmitReceive(&TOUCH_SPI_PORT, (uint8_t*)data_tx, data, sizeof(data), HAL_MAX_DELAY);
    uint16_t z1 = (((uint16_t)data[1]) << 8) | ((uint16_t)data[2]);
    uint16_t z2 = (((uint16_t)data[4]) << 8) | ((uint16_t)data[5]);
    z1 >>= 3;
    z2 >>= 3;
    uint16_t z = z1 + 4095 - z2;
    if (z >= Z_THRESHOLD) {						// The display has been touched, read the touch point coordinates
		for (uint8_t i = 0; i < 16; ++i) {
			data_tx[0] = cmd_read_Y;
			data_tx[3] = cmd_read_X;
			HAL_SPI_TransmitReceive(&TOUCH_SPI_PORT, (uint8_t*)data_tx, data, sizeof(data), HAL_MAX_DELAY);
			avg_y += (((uint16_t)data[1]) << 8) | ((uint16_t)data[2]);
			avg_x += (((uint16_t)data[4]) << 8) | ((uint16_t)data[5]);
		}
		data_tx[0] = cmd_read_YL;
		HAL_SPI_TransmitReceive(&TOUCH_SPI_PORT, (uint8_t*)data_tx, data, 3, HAL_MAX_DELAY);
    }
	HAL_GPIO_WritePin(TOUCH_CS_GPIO_Port, TOUCH_CS_Pin, GPIO_PIN_SET);

    // Restore SPI speed
#ifdef TOUCH_SPI_SPEED
	TOUCH_SPI_PORT.Init.Mode				= SPI_MODE_MASTER;
	TOUCH_SPI_PORT.Init.Direction			= SPI_DIRECTION_2LINES;
	TOUCH_SPI_PORT.Init.DataSize			= SPI_DATASIZE_8BIT;
	TOUCH_SPI_PORT.Init.CLKPolarity 		= SPI_POLARITY_LOW;
	TOUCH_SPI_PORT.Init.CLKPhase 			= SPI_PHASE_1EDGE;
	TOUCH_SPI_PORT.Init.NSS 				= SPI_NSS_SOFT;
	TOUCH_SPI_PORT.Init.BaudRatePrescaler 	= spi_speed;
	TOUCH_SPI_PORT.Init.FirstBit 			= SPI_FIRSTBIT_MSB;
	TOUCH_SPI_PORT.Init.TIMode 				= SPI_TIMODE_DISABLE;
	TOUCH_SPI_PORT.Init.CRCCalculation 		= SPI_CRCCALCULATION_DISABLE;
	TOUCH_SPI_PORT.Init.CRCPolynomial 		= 10;
	HAL_SPI_Init(&TOUCH_SPI_PORT);
#endif

	if (z >= Z_THRESHOLD) {						// The display has been touched
		avg_x += 8;								// Round result
		avg_x >>= 4;							// Divide by 16
		avg_y += 8;
		avg_y >>= 4;
		// Calculate the display configuration in non-rotated state
		*x = avg_x;
		*y = avg_y;
		return true;
	}
	return false;
}

// Translate touch screen raw coordinates to the screen pixel coordinates
bool TOUCH_GetCoordinates(uint16_t* x, uint16_t* y) {
	uint16_t raw_x, raw_y;
	if (TOUCH_GetRawData(&raw_x, &raw_y)) {
		// The touch screen calibrated in the initial mode: no rotation, calculate screen coordinates in the initial mode
		uint16_t scr_x = round(calib_data[0] * (double)raw_x + calib_data[1] * (double)raw_y + calib_data[2]);
		uint16_t scr_y = round(calib_data[3] * (double)raw_x + calib_data[4] * (double)raw_y + calib_data[5]);
		uint16_t sc_width  = TFT_Width();
		uint16_t sc_height = TFT_Height();
		switch (TFT_Rotation()) {
			case TFT_ROTATION_0:				// Initial mode, nothing to be changed
				*x = scr_x;
				*y = scr_y;
				break;
			case TFT_ROTATION_90:				// sc_width is a long side and sc_height is a short side in this mode
				*x = scr_y;
				*y = sc_height - scr_x;
				break;
			case TFT_ROTATION_180:				// invert both coordinates
				*x = sc_width  - scr_x;
				*y = sc_height - scr_y;
				break;
			case TFT_ROTATION_270:				// sc_width is a long side and sc_height is a short side in this mode
				*x = sc_width - scr_y;
				*y = scr_x;
				break;
		}
		return true;
	}
	return false;
}
