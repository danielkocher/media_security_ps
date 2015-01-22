#ifndef _HISTOGRAM_H_
#define _HISTOGRAM_H_

struct histogram_data {
	int val;
	int count;
};

struct histogram_info {
	int size;
	int pos;
};

struct histogram {
	struct histogram_info info;
	struct histogram_data *data;
};

struct histogram *histogram_init(int num_coeffs, int num_values);
void histogram_free(struct histogram *hist, int num_coeffs);

int histogram_insert_data(struct histogram *hist, int *idx_coeff, int coeff_num, int *val);
int histogram_search_data(struct histogram *hist, int *idx_coeff, int *val);

int histogram_compare(const void *p, const void *q);

#endif //_HISTOGRAM_H_