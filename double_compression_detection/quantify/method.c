#include "method.h"
#include "quant.h"

method_func m0, m1, m2, m3;

method_func *quantify_method[] = {
	m0,
	m1,
	m2,
	m3
};

// absolute difference of ascending and descending values
int m0(struct hist *hist) {
	int result = 0;
	int i;

	for(i = 1; i < hist->different_values; ++i) {
	    if(hist->value[i] <= 0) {
		  if(hist->count[i-1] > hist->count[i])
		      result += hist->count[i-1] - hist->count[i];
	    }
	    else {
		  if(hist->count[i-1] < hist->count[i])
		      result += hist->count[i] - hist->count[i-1];
	    }
	}

	return result;
}


int m1(struct hist *hist) {
	int result = 0;
	float mid = (float)hist->different_values / 2.0f;
	int i;

	for(i = 1; i < mid; ++i)
		if(hist->count[i] < hist->count[i-1])
			result += hist->count[i-1] - hist->count[i];

	for(i = mid + 1; i < hist->different_values; ++i)
		if(hist->value[i] > hist->value[i-1])
			result += hist->value[i] - hist->value[i-1];

	return result;
}

int m2(struct hist *hist) {
	int result = 0;
	int last_cmp_val = 0;
	int i;

	for(i = 0; i < hist->different_values && hist->value[i] <= 0; ++i) {
	    if(hist->count[i] < last_cmp_val)
			result += last_cmp_val - hist->count[i];
	    else
	    	last_cmp_val = hist->count[i];
	}

	for(; i < hist->different_values; ++i) {
		if(hist->count[i] > last_cmp_val)
			 result += hist->count[i] - hist->count[i-1];
		else
			last_cmp_val = hist->count[i];
	}

	return result;
}

int m3(struct hist *hist) {
	int result = 0;
	int i;

	for(i = 1; i < hist->different_values; ++i) {
	    if(hist->value[i] <= 0)
		  if(hist->count[i-1] > hist->count[i] && ((hist->count[i-1] - hist->count[i]) > THRESHOLD))
		      ++result;
	    else
		  if(hist->count[i-1] < hist->count[i] && ((hist->count[i] - hist->count[i-1]) > THRESHOLD))
		      ++result;
	}

	return result;
}