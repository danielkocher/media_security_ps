#ifndef _COEFFICIENTS_IO_H_
#define _COEFFICIENTS_IO_H_

#include <stdio.h>

#define NUM_DC_COEFFICIENTS 1
#define NUM_LP_COEFFICIENTS 15
#define NUM_HP_COEFFICIENTS 240
#define NUM_TILE_INFO 2
#define NUM_MACROBLOCK_INFO 2

struct coefficients_line {
	int tile_info[NUM_TILE_INFO];
	int macroblock_info[NUM_MACROBLOCK_INFO];
	int dc_coefficients[NUM_DC_COEFFICIENTS];
	int lp_coefficients[NUM_LP_COEFFICIENTS];
	int hp_coefficients[NUM_HP_COEFFICIENTS];
};

struct coefficients_line *coefficients_parse_line(FILE *f);
void coefficients_free(struct coefficients_line **coeffs, int num_line);

#endif // _COEFFICIENTS_IO_H_