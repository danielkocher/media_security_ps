#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <stdlib.h>

#include "quant.h"
#include "method.h"

char *resultfile = "quant.dat";
char *dir;

struct hist * read_hist(char fname[]) {
	int num, i;
	struct hist *retval;

	FILE *f = fopen(fname, "r");

	if(f == NULL) {
		printf("Couldn't open file %s", fname);
		return NULL;
	}

	for(num = 0;; ++num) {
		fscanf(f, "%d", &i);
		fscanf(f, "%d", &i);

		if(feof(f))
			break;
	}

	retval = (struct hist *)calloc(1, sizeof(struct hist));
	retval->different_values = num;
	retval->value = (int*)calloc(num, sizeof(int));
	retval->count = (int*)calloc(num, sizeof(int));

	rewind(f);

	for(i = 0; i < num; ++i) {
		fscanf(f, "%d", &retval->value[i]);
		fscanf(f, "%d", &retval->count[i]);
	}

	fclose(f);
	return retval;
}

void quantify(struct hist *hist) {
	int i;

	for(i = 0; i < NUM_METHODS; ++i)
		hist->quant[i] = quantify_method[i](hist);
}

void save_results(struct hist *hist[]) {
	int i, k;
	char *fname = calloc(strlen(dir)+10, sizeof(char));
	sprintf(fname, "%s%s", dir, resultfile);

	FILE *f = fopen(fname, "w");

	free(fname);

	if(!f) {
	    printf("Quantisation results could not be saved.");
	    return;
	}

	for(k = 0; k < NUM_HIST; ++k) {
		for(i = 0; i < NUM_METHODS; ++i) {
			fprintf(f, "%d", hist[k]->quant[i]);

			if(i < NUM_METHODS-1)
				fprintf(f, ",");
		}

		fprintf(f, "\n");
	}

	fclose(f);
}

int main (int argc, char *argv[]) {
	int i;
	char *fname;
	struct hist *hist[NUM_HIST];

	if(argc != 2) {
		printf("Usage: directory as parameter\n");
		return -1;
	}

	dir = calloc(strlen(argv[1])+2, sizeof(char));
	strcpy(dir, argv[1]);
	if(dir[strlen(dir) -1] != '/') {
		dir[strlen(dir)] = '/';
		dir[strlen(dir)+1] ='\0';
	}

	fname = calloc(strlen(dir) + 12, sizeof(char));

	/* Read all histograms of the picture */
	for(i = 0; i < NUM_HIST; ++i) {
		sprintf(fname, "%shist%d.dat", dir, i);
		hist[i] = read_hist(fname);
	}

	/* Quantify all histograms with all methods */
	for(i = 0; i < NUM_HIST; ++i)
		if(hist[i] != NULL)
			quantify(hist[i]);

	save_results(hist);

	return 0;
}
