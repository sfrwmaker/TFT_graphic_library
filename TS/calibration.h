#ifndef _CALIBRATION_H
#define _CALIBRATION_H

#include <stdint.h>
#include <stdbool.h>

#define M_DIM	(3)

typedef struct {
	uint16_t x;
	uint16_t y;
} tPOINT;

#ifdef __cplusplus
extern "C" {
#endif

void		CALB_Init(void);
bool 		CALB_add(uint16_t tx, uint16_t ty, uint16_t sx, uint16_t sy);
bool		CALB_calculate(double calib_data[6]);

#ifdef __cplusplus
}
#endif

#endif
