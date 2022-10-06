/*
 * bitmap.h
 *
 *  Created on: May 23 2020
 *      Author: Alex
 */

#ifndef _BITMAP_H_
#define _BITMAP_H_

#include "main.h"
#include <stdint.h>

/*
 * Bitmap data structure allocated in memory
 * the space to store bitmap width and height is allocated together
 * with the space for bitmap itself
 * Each bitmap instance is allocated just once
 * All other copies are just "links" to the main instance
 */
struct data {
	uint16_t	w;										// Bitmap width
	uint16_t	h;										// Bitmap height
	uint8_t		links;									// Number of the links to allocated area
	uint8_t		data[0];								// Bitmap array
};

class BITMAP {
	public:
		BITMAP(void)									{ }
		BITMAP(uint16_t width, uint16_t height);
		BITMAP(const BITMAP &bm);
		BITMAP&		operator=(const BITMAP &bm);
		~BITMAP(void);
		uint8_t*	bitmap(void)						{ return ds->data;		}
		uint16_t	width(void)							{ return (ds)?ds->w:0;	}
		uint16_t	height(void)						{ return (ds)?ds->h:0;	}
		void		clear(void);
		void		drawPixel(uint16_t x, uint16_t y);
		bool		pixel(uint16_t x, uint16_t y);
		void		drawHLine(uint16_t x, uint16_t y, uint16_t length);
		void		drawVLine(uint16_t x, uint16_t y, uint16_t length);
		void		drawIcon(uint16_t x, uint16_t y, const uint8_t *icon, uint16_t ic_width, uint16_t ic_height);
		void		drawVGauge(uint16_t gauge, bool edged);
	private:
		struct data	*ds	= 0;
};

#endif
