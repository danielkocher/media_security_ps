#include "coeffio.h"

#include <stdio.h>
#include <stdlib.h>

/**
 * Parses a whole line of comma separated coefficients into the coeff_line structure.
 *
 * Takes the file pointer as argument.
 *
 * Returns the pointer to the allocated coeff_line structure the coefficients were stored in.
 */
struct coeff_line *parse_coeff_line(FILE *f) {
	if(f == NULL || feof(f))
		return NULL;

	// allocate a single coeff_line memory block
	struct coeff_line *ret = calloc(1, sizeof(struct coeff_line));

	// holds the current character
	char current = fgetc(f);
	int pos = 0, num = 0, factor = 1;

	while(!feof(f)) {
		// check for errors
		if(ferror(f)) {
			free(ret);
			fprintf(stderr, "Error while reading file.\n");
			return NULL;
		}

		// read a single entry (entries are comma separated)
		num = current - 48;
		factor = 1;
		while(!feof(f)) {
			// consider negative sign
			if(current == '-') {
				factor = (-1);
				current = fgetc(f);
				num = current - 48;
			}

			current = fgetc(f);
			if((current == ',') || (current == '\n'))
				break;

			num = num * 10 + (current - 48);
		}
		num *= factor;

		// store coefficient into correct part of the coeff_line structure
		if(pos >= 0 && pos <= 1)
			*(ret->tile_info + pos) = num;
		else if(pos >= 2 && pos <= 3)
			*(ret->mb_info + pos - 2) = num;
		else if(pos == 4)
			*(ret->dc_coeff + pos - 4) = num;
		else if(pos >= 5 && pos <= 19)
			*(ret->lp_coeff + pos - 5) = num;
		else
			*(ret->hp_coeff + pos - 20) = num;

		if(current == '\n')
			break;

		current = fgetc(f);
		++pos;
	}

	return ret;
}

void free_coeffs(struct coeff_line **coeffs, int num_line) {
	if(coeffs == NULL)
		exit(-1);

	int i = 0;
	for(i = 0; i <= num_line; ++i) {
		if(coeffs[i] == NULL)
			exit(-1);
		
		free(coeffs[i]);
	}
	free(coeffs);
}