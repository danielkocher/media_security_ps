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

struct hist_alloc_info {
	int size;
	int pos;
};

struct histogram {
	struct hist_alloc_info info;
	struct hist_data *data;
};

struct coeff_line *parse_coeff_line(FILE *f);
void free_coeffs(struct coeff_line **coeffs, int num_line);

void update_min_max(int *current, int *min, int *max);

struct histogram *init_histogram(int num_coeffs, int num_values);
struct hist_data *init_hist_data(struct hist_alloc_info *info);
void free_histogram(struct histogram *hist, int num_coeffs);

void insert_hist_data(struct histogram *hist, int *idx_coeff, int *val);
int search_hist_data(struct histogram *hist, int *idx_coeff, int *val);

int compare(const void *p, const void *q);

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

	// number of coefficients
	int num_coeffs = NUM_LP_COEFFS;
	// at least one value present
	int num_values = 1;		

	struct histogram *hist = init_histogram(num_coeffs, num_values);

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
		for(j = 0; j < NUM_DC_COEFFS; ++j)
			fprintf(stdout, "%4d ", coeffs[i]->dc_coeff[j]);
		fprintf(stdout, "\n\n");

		fprintf(stdout, "LP:\n\t");
		for(j = 0; j < NUM_LP_COEFFS; ++j) {
			if(coeffs[i]->tile_info[0] == 0 && coeffs[i]->tile_info[1] == 0)
				insert_hist_data(hist, &j, &coeffs[i]->lp_coeff[j]);
			
			if((j > 0) && ((j % 8) == 0))
				fprintf(stdout, "\n\t");
			
			fprintf(stdout, "%4d ", coeffs[i]->lp_coeff[j]);
		}
		fprintf(stdout, "\n\n");

		fprintf(stdout, "HP:\n\t");
		for(j = 0; j < NUM_HP_COEFFS; ++j) {
			if((j > 0) && ((j % 8) == 0))
				fprintf(stdout, "\n\t");

			fprintf(stdout, "%4d ", coeffs[i]->hp_coeff[j]);
		}
		fprintf(stdout, "\n\n============================================\n\n");
	}

	f = fopen("hist.dat", "a+");
	for(i = 0; i < NUM_LP_COEFFS; ++i)
		qsort(hist[i].data, hist[i].info.size, sizeof(struct hist_data), compare);

	j = 0;
	for(i = 0; i < hist[j].info.size; ++i)
		fprintf(f, "%d %d\n", hist[j].data[i].val, hist[j].data[i].count);
	fclose(f);

	free_coeffs(coeffs, num_line);
	free_histogram(hist, num_coeffs);

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

struct histogram *init_histogram(int num_coeffs, int num_values) {
	struct histogram *hist = calloc(num_coeffs, sizeof(struct histogram));

	if(hist == NULL)
		return NULL;

	int i = 0;
	for(i = 0; i < num_coeffs; ++i) {
		hist[i].info.size = num_values;
		hist[i].info.pos = 0;

		hist[i].data = calloc(hist->info.size, sizeof(struct hist_data));

		if(hist[i].data == NULL)
			return NULL;

		hist[i].data->val = 0;
		hist[i].data->count = 0;
	}

	return hist;
}

void free_histogram(struct histogram *hist, int num_coeffs) {
	if(hist == NULL)
		return;

	int i = 0;
	for(i = 0; i < num_coeffs; ++i)
		free(hist[i].data);

	free(hist);
}

void insert_hist_data(struct histogram *hist, int *idx_coeff, int *val) {
	if(hist == NULL || idx_coeff == NULL || val == NULL) {
		fprintf(stderr, "Error in insert_hist_data.\n");
		exit(-1);
	}

	if(*idx_coeff > NUM_LP_COEFFS) {
		fprintf(stderr, "Error in insert_hist_data: num_coeff too big (%d).\n", *idx_coeff);
		exit(-1);
	}

	int pos = search_hist_data(hist, idx_coeff, val);
	if(pos < 0) {
		if((hist[*idx_coeff].info.size - 1) <= hist[*idx_coeff].info.pos) {
			hist[*idx_coeff].info.size *= 2;
			hist[*idx_coeff].data = realloc(hist[*idx_coeff].data, hist[*idx_coeff].info.size * sizeof(struct hist_data));

			if(hist[*idx_coeff].data == NULL) {
				fprintf(stderr, "Error while reallocing histogram data to size %d.\n", hist[*idx_coeff].info.pos);
				exit(-1);
			}

			int i = 0;
			for(i = (hist[*idx_coeff].info.pos + 1); i < hist[*idx_coeff].info.size; ++i) {
				hist[*idx_coeff].data[i].val = 0;
				hist[*idx_coeff].data[i].count = 0;
			}
		}
		
		hist[*idx_coeff].data[hist[*idx_coeff].info.pos].val = *val;
		hist[*idx_coeff].data[hist[*idx_coeff].info.pos].count = 1;
		++(hist[*idx_coeff].info.pos);
	} else {
		++(hist[*idx_coeff].data[pos].count);
	}
}


int search_hist_data(struct histogram *hist, int *idx_coeff, int *val) {
	if(hist == NULL || idx_coeff == NULL || val == NULL) {
		fprintf(stderr, "Error in search_hist_data.\n");
		exit(-1);
	}

	int i = 0;
	for(i = 0; i <= hist[*idx_coeff].info.pos; ++i)
		if(hist[*idx_coeff].data[i].val == *val)
			return i;

	return -1;
}

int compare(const void *p, const void *q) {
	struct hist_data *sp = ((struct hist_data *)p);
	struct hist_data *sq = ((struct hist_data *)q);

	return (sp->val > sq->val);
}