#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include <unistd.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <dirent.h>

#include "histogram.h"
#include "coeffio.h"

struct dynamic_string_array {
	int allocated;
	int count;
	char **strings;
};

void list_directory(struct dynamic_string_array *dsa, const char *directory_name);
void cleanup(pid_t *pids_children, struct dynamic_string_array *dsa);

int main(int argc, char** argv) {
	struct dynamic_string_array *dsa = calloc(1, sizeof(struct dynamic_string_array));
	int i = 0, j = 0, k = 0;

	dsa->allocated = 1;
	dsa->count = 0;
	dsa->strings = calloc(dsa->allocated, sizeof(char *));

	list_directory(dsa, "./test_images");

	int num_of_children = dsa->count;
	fprintf(stdout, "Creating %d example child processes.\n", num_of_children);

	pid_t *pids_children = NULL;
	pid_t pid_child = -1;
	pids_children = calloc(num_of_children, sizeof(pid_t));
	if(pids_children == NULL) {
		fprintf(stderr, "An error occurred while allocating %d PIDs!\n", num_of_children);

		for(i = 0; i < dsa->count; ++i)
			if(dsa->strings[i] != NULL)
				free(dsa->strings[i]);
		
		if(dsa->strings != NULL)
			free(dsa->strings);
		if(dsa != NULL)
			free(dsa);

		return (-1);
	}

	char *fn_tif = NULL;
	i = 0;
	for(i = 0; i < num_of_children; ++i) {
		if((pid_child = fork()) == 0) {
			fn_tif = calloc(1, strlen(dsa->strings[i]) + 1);
			strncpy(fn_tif, dsa->strings[i], strlen(dsa->strings[i]) + 1);
			fn_tif[strlen(fn_tif) - 3] = 't';
			fn_tif[strlen(fn_tif) - 2] = 'i';
			fn_tif[strlen(fn_tif) - 1] = 'f';

			char *cmd[] = {"jpegxr", "-o", fn_tif, dsa->strings[i], (char *)0};
			int ret = execv("T.835/jpegxr", cmd);

			if(ret == -1)
				cleanup(pids_children, dsa);

			return ret;
		}
		else {
			pids_children[i] = pid_child;
		}
	}

	int waiting_for_children = 0;
	do {
		waiting_for_children = 0;
		for(i = 0; i < num_of_children; ++i) {
			if(pids_children[i] > 0) {
				if(waitpid(pids_children[i], NULL, WNOHANG) != 0) {
					fprintf(stdout, "Child process %d is done.\n", pids_children[i]);
					char *last_slash = strrchr(dsa->strings[i], '/');
					const char *coeffs_fn = "coeffs.csv";
					char *path_coeff_file = calloc(last_slash - dsa->strings[i] + strlen(coeffs_fn) + 2, sizeof(char));

					if(path_coeff_file == NULL) {
						fprintf(stderr, "Could not allocate memory for coefficient path.\n");
						return -1;
					}

					strncpy(path_coeff_file, dsa->strings[i], last_slash - dsa->strings[i] + 1);
					strncat(path_coeff_file, coeffs_fn, strlen(coeffs_fn));

					FILE *f = fopen(path_coeff_file, "r");

					int coeffs_size = 100;
					struct coeff_line **coeffs = calloc(coeffs_size, sizeof(struct coeff_line *));
					int num_line = 0;

					if(f == NULL) {
						fprintf(stderr, "Could not open file %s.", path_coeff_file);
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

					for(j = 0; j < num_line; ++j)
						for(k = 0; k < NUM_LP_COEFFS; ++k)
							insert_hist_data(hist, &k, NUM_LP_COEFFS, &coeffs[j]->lp_coeff[k]);

					// resize to minimum size
					for(j = 0; j < NUM_LP_COEFFS; ++j) {
						if(hist[j].info.pos < hist[j].info.size) {
							hist[j].info.size = hist[j].info.pos;
							--hist[j].info.pos;
							hist[j].data = realloc(hist[j].data, hist[j].info.size * sizeof(struct hist_data *));
						}
					}

					// sort coefficients
					for(j = 0; j < NUM_LP_COEFFS; ++j)
						qsort(hist[j].data, hist[j].info.size, sizeof(struct hist_data), compare_hist);

					//char **fns = calloc(NUM_LP_COEFFS, sizeof(char *));
					for(j = 0; j < NUM_LP_COEFFS; ++j) {
						char *hist_fn = calloc(strlen("hist.dat") + 3 + 4, sizeof(char));
						char *path_hist_file = calloc(last_slash - dsa->strings[i] + strlen("hist.dat") + 3 + 1, sizeof(char));

						snprintf(hist_fn, (strlen("hist.dat") + 3), "hist%d.dat", j);
						//fprintf(stdout, "Filename: %s\n", fn);

						strncpy(path_hist_file, dsa->strings[i], last_slash - dsa->strings[i] + 1);
						strncat(path_hist_file, hist_fn, strlen(hist_fn));
						
						free(hist_fn);
						
						f = fopen(path_hist_file, "a+");
						for(k = 0; k < hist[j].info.size; ++k)
							fprintf(f, "%d %d\n", hist[j].data[k].val, hist[j].data[k].count);
						fclose(f);

						//fns[j] = path_hist_file;
						free(path_hist_file);
					}

					/*
					for(j = 0; j < NUM_LP_COEFFS; ++j)
						if(fns[j] != NULL)
							free(fns[j]);
					free(fns);
					*/

					free_coeffs(coeffs, num_line);
					free_histogram(hist, num_coeffs);
					fprintf(stdout, "Histogram data is written.\n");

					pids_children[i] = 0;
				}
				else {
					//fprintf(stdout, "Still waiting on example child process %d by now!\n", pids_children[i]);
					waiting_for_children = 1;
				}
			}
		}

		sleep(0);
	}while(waiting_for_children != 0);

	return 0;
}

char *dynamic_string_array_insert(struct dynamic_string_array *dsa, int string_size) {
	if(dsa->count >= dsa->allocated) {
		dsa->allocated *= 2;
		dsa->strings = realloc(dsa->strings, dsa->allocated * sizeof(char *));
	}
		
	char *string = calloc(string_size, sizeof(char) + 1);
	dsa->strings[dsa->count++] = string;

	return string;
}

void cleanup(pid_t *pids_children, struct dynamic_string_array *dsa) {
	if(pids_children != NULL)
		free(pids_children);

	int i = 0;
	for(i = 0; i < dsa->count; ++i)
		if(dsa->strings[i] != NULL)
			free(dsa->strings[i]);
	
	if(dsa->strings != NULL)
		free(dsa->strings);
	if(dsa != NULL)
		free(dsa);
}

void list_directory(struct dynamic_string_array *dsa, const char *directory_name) {
	DIR *directory = NULL;
	struct dirent *entry = NULL;

	// open directory
	directory = opendir(directory_name);

	// check success
	if(directory == NULL) {
		fprintf(stderr, "Could not open directory ./files/\n");
		exit(-1);
	}

	while(1) {
		// read directory
		entry = readdir(directory);

		// check success
		if(entry == NULL)
			break;

		if(	(strcmp(entry->d_name, "..") != 0)
			&& (strcmp(entry->d_name, ".") != 0))
		{
			int path_length = strlen(directory_name) + strlen(entry->d_name) + 2; // +2 because of the / and the \0
			char *path = calloc(path_length, sizeof(char));

			path_length = snprintf(path, path_length, "%s/%s", directory_name, entry->d_name);
			if(path_length >= PATH_MAX) {
				fprintf(stderr, "Path length exceeded!\n");
				exit(-1);
			}

			// entry is directory
			if(entry->d_type & DT_DIR) {
				list_directory(dsa, path);
			} else { // entry is no directory
				char *entry = dynamic_string_array_insert(dsa, path_length + 1);
				memcpy(entry, path, path_length + 1);
			}
			free(path);
		}
	}

	if(closedir(directory)) {
		fprintf(stderr, "Could not close %s\n", directory_name);
		exit(-1);
	}
}