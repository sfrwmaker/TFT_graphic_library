#include "calibration.h"
#include <math.h>

#define MAX_POINT_NUM	(10)

static tPOINT 	touch[MAX_POINT_NUM];	// Coordinates of touch-screen
static tPOINT 	scren[MAX_POINT_NUM];	// Coordinates of screen
static uint8_t	points = 0;				// number of reference points
const double error = 1.0e-10;

// Forward function declarations
static bool findNonZeroLine(double m[M_DIM][M_DIM+1], uint8_t x);
static bool extMatrix(double m[M_DIM][M_DIM+1]);

void CALB_Init(void) {
	points = 0;
}


bool CALB_add(uint16_t tx, uint16_t ty, uint16_t sx, uint16_t sy) {
	if (points >= MAX_POINT_NUM)
		return false;
	touch[points].x	= tx;
	touch[points].y	= ty;
	scren[points].x	= sx;
	scren[points].y	= sy;
	++points;
	return true;
}

// calibration data is the array of calibration coefficients
// calib_data[0] = A, calib_data[1] = B, calib_data[2] = C, ... calib_data[5] = F
// screen coordinates (x, y) can be calculated from touch coordinates (X, Y) as:
// x = A*X + B*Y + C
// y = D*X + E*Y + F
//
// To find the calibration coefficients A, B, C, D, E, F, we have to
// collect calibration data on several points (Xi, Yi) -> (xi, yi)
// and solve two linear equation systems (N is a number of reference points):
// 1.
// sum(Xi*Xi) * A + sum(Xi*Yi) * B + sum(Xi) * C = sum(Xi*xi);	(1.1)
// sum(Xi*Yi) * A + sum(Yi*Yi) * B + sum(Yi) * C = sum(Yi*xi);	(1.2)
// sum(Xi)    * A +    sum(Yi) * B +       N * C = sum(xi);		(1.3)
// 2.
// sum(Xi*Xi) * D + sum(Xi*Yi) * E + sum(Xi) * F = sum(Xi*yi);	(2.1)
// sum(Xi*Yi) * D + sum(Yi*Yi) * E + sum(Yi) * F = sum(Yi*yi);	(2.2)
// sum(Xi)    * D +    sum(Yi) * E +      N  * F = sum(yi);		(2.3)
bool CALB_calculate(double calib_data[6]) {
	if (points < 4)
		return false;

	uint32_t sX 	= 0;				// Sum(Xi) (touch panel)
	uint32_t sY 	= 0;				// Sum(Yi) (touch panel)
	uint32_t sx 	= 0;				// Sum(xi) (screen)
	uint32_t sy		= 0;				// Sum(yi) (screen)
	uint32_t sXY	= 0;				// Sum(Xi*Yi)
	uint32_t sXX	= 0;				// Sum(Xi*Xi)
	uint32_t sYY	= 0;				// Sum(Yi*Yi)
	uint32_t sxX	= 0;				// Sum(Xi*xi)
	uint32_t sxY	= 0;				// Sum(Yi*xi)
	uint32_t syX	= 0;				// Sum(Xi*yi)
	uint32_t syY	= 0;				// Sum(Yi*yi)

	for (uint8_t i = 0; i < points; ++i) {
		sX	+=	touch[i].x;
		sY	+=	touch[i].y;
		sx	+=	scren[i].x;
		sy	+=	scren[i].y;
		sXY	+=	touch[i].x * touch[i].y;
		sXX	+=	touch[i].x * touch[i].x;
		sYY	+=	touch[i].y * touch[i].y;
		sxX	+=	scren[i].x * touch[i].x;
		sxY	+=	scren[i].x * touch[i].y;
		syX	+=	scren[i].y * touch[i].x;
		syY	+=	scren[i].y * touch[i].y;
	}
	// Calculate A, B, C coefficients
	double m[M_DIM][M_DIM+1];
	m[0][0]	= (double)sXX;				// (1.1)
	m[0][1]	= (double)sXY;
	m[0][2]	= (double)sX;
	m[0][3]	= (double)sxX;
	m[1][0]	= (double)sXY;				// (1.2)
	m[1][1]	= (double)sYY;
	m[1][2]	= (double)sY;
	m[1][3]	= (double)sxY;
	m[2][0]	= (double)sX;				// (1.3)
	m[2][1]	= (double)sY;
	m[2][2]	= (double)points;
	m[2][3]	= (double)sx;
	if (extMatrix(m)) {
		for (uint8_t i = 0; i < M_DIM; ++i) {
			calib_data[i] = m[i][M_DIM];
		}
	} else {
		return false;
	}
	// Calculate D, E, F coefficients
	m[0][0]	= (double)sXX;				// (2.1)
	m[0][1]	= (double)sXY;
	m[0][2]	= (double)sX;
	m[0][3]	= (double)syX;
	m[1][0]	= (double)sXY;				// (2.2)
	m[1][1]	= (double)sYY;
	m[1][2]	= (double)sY;
	m[1][3]	= (double)syY;
	m[2][0]	= (double)sX;				// (2.3)
	m[2][1]	= (double)sY;
	m[2][2]	= (double)points;
	m[2][3]	= (double)sy;
	if (extMatrix(m)) {
		for (uint8_t i = 0; i < M_DIM; ++i) {
			calib_data[i+3] = m[i][M_DIM];
		}
		return true;
	}
	return false;
}

static bool notZero(double x) {
	return (fabs(x) > error);
}

// Make sure m[x][x] is non-zero, otherwise search for another line and change lines
static bool findNonZeroLine(double m[M_DIM][M_DIM+1], uint8_t x) {
	for (uint8_t i = x; i < M_DIM; ++i) {
		if (notZero(m[i][x])) {				// Found line that has non-zero m[i][x]
			if (x == i) {					// m[x][x] is non-zero
				return true;
			} else {						// Change lines i & x
				for (uint8_t k = 0; k < M_DIM+1; ++k) {
					double t	= m[i][k];
					m[x][k] 	= m[i][k];
					m[i][k]		= t;
				}
				return true;				// now m[x][x] is non-zero
			}
		}
	}
	return false;
}

// Solves the extended matrix of linear equations by Gauss method
// The answer is a column m[M_DIM][i]
//static bool extMatrix(double m[M_DIM][M_DIM+1]) {
static bool extMatrix(double m[M_DIM][M_DIM+1]) {
	for (uint8_t i = 0; i < M_DIM; ++i) {
		if (!findNonZeroLine(m, i))
			return false;					// Cannot solve the problem because all members m[l][i] are zero!
		double a = m[i][i];					// non-zero for sure
		for (uint8_t j = i+1; j < M_DIM; ++j) {
			double b = m[j][i];
			if (notZero(b)) {				// If non-zero only; after loop m[j][i] become zero
				for (uint8_t k = 0; k <= M_DIM; ++k) {
					m[j][k] *= a;
					m[j][k] -= m[i][k] * b;
				}
			}
		}
		// make m[i][i] equal to 1
		for (uint8_t k = 0; k <= M_DIM; ++k) {
			m[i][k] /= a;
		}
	}
	// Here we have a triangle matrix:
	// 1 x x | y
	// 0 1 x | y
	// 0 0 1 | y
	for (int8_t i = M_DIM-1; i >= 0; --i) {
		for (int8_t j = i-1; j >= 0; --j) {
			double b = m[j][i];
			if (notZero(b)) {				// If non-zero only; after loop m[j][i] become zero
				for (uint8_t k = 0; k <= M_DIM; ++k) {
					m[j][k] -= m[i][k] * b;
				}
			}
		}
	}
	// Here we have a diagonal matrix:
	// 1 0 0 | z
	// 0 1 0 | z
	// 0 0 1 | z
	return true;
}

/*
static uint32_t Random(uint32_t min, uint32_t max) {
	if (min < max) {
		return rand() % (max-min) + min;
	}
	return min;
}

// Test linear equation system
uint16_t testMatrix(uint16_t iterations) {
	for (uint16_t i = 0; i < iterations; ++i) {
		double A = Random(1, 100);
		double B = Random(1, 100);
		double C = Random(1, 100);

		double m[M_DIM][M_DIM+1];
		m[0][0]		= 11.0;
		m[0][1]		= 13.0;
		m[0][2]		= 17.0;
		m[0][3]		= m[0][0] * A + m[0][1] * B + m[0][2] * C;
		m[1][0]		= 23.0;
		m[1][1]		= 27.0;
		m[1][2]		= 31.0;
		m[1][3]		= m[1][0] * A + m[1][1] * B + m[1][2] * C;
		m[2][0]		= 33.0;
		m[2][1]		= 37.0;
		m[2][2]		= 41.0;
		m[2][3]		= m[2][0] * A + m[2][1] * B + m[2][2] * C;

		if (extMatrix(m)) {
			if (notZero(A - m[0][3]) || notZero(B - m[1][3]) || notZero(C - m[2][3]))
				return i;
		}
	}
	return iterations;
}
*/
