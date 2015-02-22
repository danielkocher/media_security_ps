#ifndef QUANT_H_
#define QUANT_H_

#define NUM_HIST 16
#define NUM_METHODS 3

struct hist
{
	int different_values;
	int *value;
	int *count;
	int quant[NUM_METHODS];
};

#endif /* QUANT_H_ */

