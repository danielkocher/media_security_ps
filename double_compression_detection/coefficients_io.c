#include "coefficients_io.h"

#include <stdio.h>
#include <stdlib.h>

/**
 * Parses a whole line of comma separated coefficients into the coeff_line structure.
 *
 * Takes the file pointer as argument.
 *
 * Returns the pointer to the allocated coeff_line structure the coefficients were stored in.
 */
struct coefficients_line *coefficients_parse_line(FILE *f) {
	if(f == NULL || feof(f))
		return NULL;

	// allocate a single coeff_line memory block
	struct coefficients_line *ret = calloc(1, sizeof(struct coefficients_line));

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
			*(ret->macroblock_info + pos - 2) = num;
		else if(pos == 4)
			*(ret->dc_coefficients + pos - 4) = num;
		else if(pos >= 5 && pos <= 19)
			*(ret->lp_coefficients + pos - 5) = num;
		else
			*(ret->hp_coefficients + pos - 20) = num;

		if(current == '\n')
			break;

		current = fgetc(f);
		++pos;
	}

	return ret;
}

void coefficients_free(struct coefficients_line **coeffs, int num_line) {
	if(coeffs == NULL)
		return;

	int i = 0;
	for(i = 0; i <= num_line; ++i) {
		if(coeffs[i] != NULL)
			free(coeffs[i]);
	}

	free(coeffs);
}