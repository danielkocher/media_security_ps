#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include <unistd.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/wait.h>

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

struct hist_data {
	int val;
	int count;
};

struct coeff_line *parse_coeff_line(FILE *f);
void free_coeffs(struct coeff_line **coeffs, int num_line);
void update_min_max(int *current, int *min, int *max);
void insert_hist_data(struct hist_data *hist, int *hist_size, int *hist_pos, int val);
int search_hist_data(struct hist_data *hist, int *hist_pos, int val);

int main(int argc, char** argv) {
	if(argc < 3) {
		fprintf(stderr, "USAGE: main <input file> <output file>\n");
		return -1;
	}

	int ret;
	char *path_input_file = argv[1];
	char *path_output_file = argv[2];
	char *cmd[] = {"jpegxr", "-o", path_output_file, path_input_file, (char *)0};
	pid_t child_pid;
	int child_status;
	int i = 0, j = 0;

	child_pid = fork();

	if(child_pid == 0) {
		ret = execv("T.835/jpegxr", cmd);
		return ret; // should never be reached
	} else {
		pid_t term_pid;
		do {
			term_pid = wait(&child_status);
		}while(term_pid != child_pid);
	}

	const char *path_coeff_file = "coeffs.csv";
	FILE *f = fopen(path_coeff_file, "r");

	int coeffs_size = 100;
	struct coeff_line **coeffs = calloc(coeffs_size, sizeof(struct coeff_line *));
	int num_line = 0;

	if(f == NULL) {
		fprintf(stderr, "Could not open file %s.", path_coeff_file);
		fclose(f);
		free(coeffs);
		return -1;
	}

	while(!feof(f)) {
		// check for size of array and allocate more if necessary
		if(num_line >= (coeffs_size - 1)) {
			coeffs_size *= 2;
			coeffs = realloc(coeffs, coeffs_size * sizeof(struct coeff_line *));
		}

		// parse a whole line of coefficients and store pointer
		*(coeffs + num_line) = parse_coeff_line(f);

		++num_line;
	}

	// resize to minimum size
	if(num_line < coeffs_size) {
		coeffs_size = num_line;
		--num_line;
		coeffs = realloc(coeffs, coeffs_size * sizeof(struct coeff_line *));
	}

	// close file if it was opened
	if(f != NULL)
		fclose(f);

	int hist_size = num_line;
	int hist_pos = 0;
	struct hist_data *hist = calloc(hist_size, sizeof(struct hist_data));

	//fprintf(stdout, "File %s closed.\n", path_coeff_file);

	// debug output of read coefficients in format to execute a diff -u on the original coeffs.csv
	/*
	for(i = 0; i < num_line; ++i) {
		for(j = 0; j < NUM_TILE_INFO; ++j)
			fprintf(stdout, "%d,", coeffs[i]->tile_info[j]);

		for(j = 0; j < NUM_MB_INFO; ++j)
			fprintf(stdout, "%d,", coeffs[i]->mb_info[j]);

		for(j = 0; j < NUM_DC_COEFFS; ++j)
			fprintf(stdout, "%d,", coeffs[i]->dc_coeff[j]);

		for(j = 0; j < NUM_LP_COEFFS; ++j)
			fprintf(stdout, "%d,", coeffs[i]->lp_coeff[j]);

		for(j = 0; j < (NUM_HP_COEFFS - 1); ++j)
			fprintf(stdout, "%d,", coeffs[i]->hp_coeff[j]);
		fprintf(stdout, "%d\n", coeffs[i]->hp_coeff[(NUM_HP_COEFFS - 1)]);
	}
	*/

	int max = INT_MIN;
	int min = INT_MAX;

	for(i = 0; i < num_line; ++i) {
		fprintf(stdout, "TILE:\n\t");
		for(j = 0; j < NUM_TILE_INFO; ++j)
			fprintf(stdout, "%4d ", coeffs[i]->tile_info[j]);
		fprintf(stdout, "\n\n");

		fprintf(stdout, "MACROBLOCK:\n\t");
		for(j = 0; j < NUM_MB_INFO; ++j)
			fprintf(stdout, "%4d ", coeffs[i]->mb_info[j]);
		fprintf(stdout, "\n\n");

		fprintf(stdout, "DC:\n\t");
		for(j = 0; j < NUM_DC_COEFFS; ++j) {
			update_min_max(&coeffs[i]->dc_coeff[j], &min, &max);
			fprintf(stdout, "%4d ", coeffs[i]->dc_coeff[j]);
		}
		fprintf(stdout, "\n\n");

		fprintf(stdout, "LP:\n\t");
		for(j = 0; j < NUM_LP_COEFFS; ++j) {
			update_min_max(&coeffs[i]->lp_coeff[j], &min, &max);

			if(j == 0)
				insert_hist_data(hist, &hist_size, &hist_pos, coeffs[i]->lp_coeff[j]);
			
			if((j > 0) && ((j % 8) == 0))
				fprintf(stdout, "\n\t");
			
			fprintf(stdout, "%4d ", coeffs[i]->lp_coeff[j]);
		}
		fprintf(stdout, "\n\n");

		fprintf(stdout, "HP:\n\t");
		for(j = 0; j < NUM_HP_COEFFS; ++j) {
			update_min_max(&coeffs[i]->hp_coeff[j], &min, &max);

			if((j > 0) && ((j % 8) == 0))
				fprintf(stdout, "\n\t");

			fprintf(stdout, "%4d ", coeffs[i]->hp_coeff[j]);
		}
		fprintf(stdout, "\n\n============================================\n\n");
	}
	fprintf(stdout, "min = %4d\nmax = %4d\n", min, max);

	// resize to minimum size
	if(hist_pos < hist_size) {
		hist_size = hist_pos;
		--hist_pos;
		hist = realloc(hist, hist_size * sizeof(struct hist_data));
	}

	for(i = 0; i < hist_pos; ++i)
		fprintf(stdout, "hist[%d].val = %d, hist[%d].count = %d\n", i, hist[i].val, i, hist[i].count);

	free_coeffs(coeffs, num_line);
	free(hist);

	return 0;
}

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

void update_min_max(int *current, int *min, int *max) {
	if(current == NULL || min == NULL || max == NULL)
		exit(-1);

	if(*current > *max) {
		*max = *current;
		return;
	}
	else if(*current < *min) {
		*min = *current;
	}
}

void insert_hist_data(struct hist_data *hist, int *hist_size, int *hist_pos, int val) {
	if(hist == NULL || hist_size == NULL || hist_pos == NULL)
		exit(-1);

	int pos = search_hist_data(hist, hist_pos, val);
	if(pos < 0) {
		hist[*hist_pos].val = val;
		hist[*hist_pos].count = 1;
		++(*hist_pos);
	} else {
		++(hist[pos].count);
	}
}

int search_hist_data(struct hist_data *hist, int *hist_pos, int val) {
	if(hist == NULL || hist_pos == NULL)
		exit(-1);

	int i = 0;
	for(i = 0; i <= *hist_pos; ++i)
		if(hist[i].val == val)
			return i;

	return -1;
}