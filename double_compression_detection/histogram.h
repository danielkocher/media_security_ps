#ifndef _HISTOGRAM_H_
#define _HISTOGRAM_H_

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

struct histogram *init_histogram(int num_coeffs, int num_values);
struct hist_data *init_hist_data(struct hist_alloc_info *info);
void free_histogram(struct histogram *hist, int num_coeffs);

int insert_hist_data(struct histogram *hist, int *idx_coeff, int coeff_num, int *val);
int search_hist_data(struct histogram *hist, int *idx_coeff, int *val);

int compare_hist(const void *p, const void *q);

#endif //_HISTOGRAM_H_