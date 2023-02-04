/*
 * ts.h
 *
 *  Created on: Nov 16, 2022
 *      Author: Alex
 */

#ifndef _TS_H_
#define _TS_H_

#include "main.h"
#include "tft.h"

#ifdef WITH_TOUCH
#include "touch.h"

void Touch(tRotation rotation);
void TouchCalibrate(void);

#endif					// WITH_TOUCH

#endif					// _TS_H_
