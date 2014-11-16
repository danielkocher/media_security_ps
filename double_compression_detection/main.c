#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

char *read_line(FILE *f);

int main(int argc, char** argv) {
	int ret;
	char *path = "../pictures/lena512.jxr";
	char *cmd[] = {"jpegxr", path, (char *)0};
	pid_t child_pid;
	int child_status;

	child_pid = fork();

	if(child_pid == 0) {
		ret = execv("T.835/jpegxr", cmd);
		return -1; // should never be reached
	} else {
		pid_t term_pid;
		do {
			term_pid = wait(&child_status);
		}while(term_pid != child_pid);
	}

	FILE *f = fopen("coeffs.csv", "r");
	printf("Tried to open file.\n");

	if(f == NULL) {
		fprintf(stderr, "Could not open file %s.", path);
		return -1;
	}

	printf("File opened. Reading first line.\n");
	char *buf = read_line(f);
	printf("%s\n", buf);

	if(f != NULL)
		fclose(f);

	return ret;
}

char *read_line(FILE *f) {
	if(f == NULL)
		return NULL;

	int size = 1000;
	char *buf = calloc(size, sizeof(char));
	char current = fgetc(f);
	int pos = 0;

	while(!feof(f) && current != '\n') {
		// check for errors
		if(ferror(f)) {
			fprintf(stderr, "Error while reading file.\n");
			return NULL;
		}

		// check for size of array and allocate more if necessary
		if(pos >= size) {
			size *= 2;
			buf = realloc(buf, size);
		}

		// store next character
		*(buf + pos) = current;
		current = fgetc(f);
		++pos;
	}

	if(pos >= size) {
		++size;
		buf = realloc(buf, size);
	}

	*(buf + pos) = '\0';

	if(pos < size) {
		size = pos + 1;
		buf = realloc(buf, size);
	}

	return buf;
}