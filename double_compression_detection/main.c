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
#include "coefficients_io.h"
#include "dynamic_string_array.h"

void list_directory(struct dynamic_string_array *dsa, const char *directory_name);
void cleanup(pid_t *pids_children, struct dynamic_string_array *dsa);

int main(int argc, char** argv) {
	if(argc < 2) {
		fprintf(stderr, "USAGE: ./main <path to test images>");
		return -1;
	}

	char *path_test_images = argv[1];
	struct dynamic_string_array *dsa = calloc(1, sizeof(struct dynamic_string_array));
	int i = 0, j = 0, k = 0;

	dsa->allocated = 1;
	dsa->count = 0;
	dsa->strings = calloc(dsa->allocated, sizeof(char *));

	list_directory(dsa, path_test_images);

	int num_of_children = dsa->count;
	fprintf(stdout, "Creating %d example child processes.\n", num_of_children);

	pid_t *pids_children = NULL;
	pid_t pid_child = -1;
	pids_children = calloc(num_of_children, sizeof(pid_t));
	if(pids_children == NULL) {
		fprintf(stderr, "An error occurred while allocating %d PIDs!\n", num_of_children);
		cleanup(pids_children, dsa);
		return (-1);
	}

	char *fn_tif = NULL;
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
					struct coefficients_line **coeffs = calloc(coeffs_size, sizeof(struct coefficients_line *));
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
							coeffs = realloc(coeffs, coeffs_size * sizeof(struct coefficients_line *));
						}

						// parse a whole line of coefficients and store pointer
						*(coeffs + num_line) = coefficients_parse_line(f);
						++num_line;
					}

					// resize to minimum size
					if(num_line < coeffs_size) {
						coeffs_size = num_line;
						--num_line;
						coeffs = realloc(coeffs, coeffs_size * sizeof(struct coefficients_line *));
					}

					// close file if it was opened
					if(f != NULL)
						fclose(f);

					// number of coefficients
					int num_coeffs = NUM_DC_COEFFICIENTS + NUM_LP_COEFFICIENTS; // + NUM_HP_COEFFICIENTS;
					int dc_max_idx = NUM_DC_COEFFICIENTS;
					int lp_max_idx = dc_max_idx + NUM_LP_COEFFICIENTS;
					int hp_max_idx = lp_max_idx + NUM_HP_COEFFICIENTS;
					// at least one value present
					int num_values = 1;		
					struct histogram *hist = histogram_init(num_coeffs, num_values);

					for(j = 0; j < num_line; ++j) {
						for(k = 0; k < dc_max_idx; ++k)
							histogram_insert_data(hist, &k, dc_max_idx, &coeffs[j]->dc_coefficients[k]);
						for(k = dc_max_idx; k < lp_max_idx; ++k)
							histogram_insert_data(hist, &k, lp_max_idx, &coeffs[j]->lp_coefficients[k - dc_max_idx]);
						//for(k = lp_max_idx; k < hp_max_idx; ++k)
						//	histogram_insert_data(hist, &k, hp_max_idx, &coeffs[j]->hp_coefficients[k - lp_max_idx]);
					}

					// resize to minimum size
					for(j = 0; j < num_coeffs; ++j) {
						if(hist[j].info.pos < hist[j].info.size) {
							hist[j].info.size = hist[j].info.pos;
							--hist[j].info.pos;
							hist[j].data = realloc(hist[j].data, hist[j].info.size * sizeof(struct hist_data *));
						}
					}

					// sort coefficients
					for(j = 0; j < num_coeffs; ++j)
						qsort(hist[j].data, hist[j].info.size, sizeof(struct histogram_data), histogram_compare);

					//char **fns = calloc(NUM_LP_COEFFS, sizeof(char *));
					for(j = 0; j < num_coeffs; ++j) {
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

						free(path_hist_file);
					}

					coefficients_free(coeffs, num_line);
					histogram_free(hist, num_coeffs);
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

void cleanup(pid_t *pids_children, struct dynamic_string_array *dsa) {
	if(pids_children != NULL)
		free(pids_children);

	dynamic_string_array_free(dsa);
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
			&& (strcmp(entry->d_name, ".") != 0)
			&& (strcmp(entry->d_name, "generate_histograms.sh") != 0)
			&& (strcmp(entry->d_name, "hist.gnu") != 0))
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