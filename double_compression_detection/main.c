#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include <unistd.h>

int main(int argc, char** argv) {
	int ret;
	char *cmd[] = {"jpegxr", "../pictures/lena512.jxr", (char *)0};
	ret = execv("T.835/jpegxr", cmd);

	return ret;
}