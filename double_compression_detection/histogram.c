#include "histogram.h"

#include <stdio.h>
#include <stdlib.h>

struct histogram *histogram_init(int num_coeffs, int num_values) {
	struct histogram *hist = calloc(num_coeffs, sizeof(struct histogram));

	if(hist == NULL)
		return NULL;

	int i = 0;
	for(i = 0; i < num_coeffs; ++i) {
		hist[i].info.size = num_values;
		hist[i].info.pos = 0;

		hist[i].data = calloc(hist->info.size, sizeof(struct histogram_data));

		if(hist[i].data == NULL)
			return NULL;

		hist[i].data->val = 0;
		hist[i].data->count = 0;
	}

	return hist;
}

void histogram_free(struct histogram *hist, int num_coeffs) {
	if(hist == NULL)
		return;

	int i = 0;
	for(i = 0; i < num_coeffs; ++i)
		free(hist[i].data);

	free(hist);
}

int histogram_insert_data(struct histogram *hist, int *idx_coeff, int coeff_num, int *val) {
	if(hist == NULL || idx_coeff == NULL || val == NULL) {
		fprintf(stderr, "Error in insert_hist_data.\n");
		return -1;
	}

	if(*idx_coeff > coeff_num) {
		fprintf(stderr, "Error in insert_hist_data: num_coeff too big (%d).\n", *idx_coeff);
		return -1;
	}

	int pos = histogram_search_data(hist, idx_coeff, val);
	if(pos < 0) {
		if((hist[*idx_coeff].info.size - 1) <= hist[*idx_coeff].info.pos) {
			hist[*idx_coeff].info.size *= 2;
			hist[*idx_coeff].data = realloc(hist[*idx_coeff].data, hist[*idx_coeff].info.size * sizeof(struct histogram_data));

			if(hist[*idx_coeff].data == NULL) {
				fprintf(stderr, "Error while reallocing histogram data to size %d.\n", hist[*idx_coeff].info.pos);
				return -1;
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

	return 0;
}


int histogram_search_data(struct histogram *hist, int *idx_coeff, int *val) {
	if(hist == NULL || idx_coeff == NULL || val == NULL) {
		fprintf(stderr, "Error in search_hist_data.\n");
		return -1;
	}

	int i = 0;
	for(i = 0; i <= hist[*idx_coeff].info.pos; ++i)
		if(hist[*idx_coeff].data[i].count > 0 && hist[*idx_coeff].data[i].val == *val)
			return i;

	return -1;
}

int histogram_compare(const void *p, const void *q) {
	return ((((struct histogram_data *)p)->val) > (((struct histogram_data *)q)->val));
}