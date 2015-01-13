#ifndef _COEFFIO_H_
#define _COEFFIO_H_

#include <stdio.h>

#define NUM_DC_COEFFS 1
#define NUM_LP_COEFFS 15
#define NUM_HP_COEFFS 240
#define NUM_TILE_INFO 2
#define NUM_MB_INFO 2

struct coeff_line {
	int tile_info[NUM_TILE_INFO];
	int mb_info[NUM_MB_INFO];
	int dc_coeff[NUM_DC_COEFFS];
	int lp_coeff[NUM_LP_COEFFS];
	int hp_coeff[NUM_HP_COEFFS];
};

struct coeff_line *parse_coeff_line(FILE *f);
void free_coeffs(struct coeff_line **coeffs, int num_line);

#endif // _COEFFIO_H_