#include "dynamic_string_array.h"

#include <stdlib.h>

char *dynamic_string_array_insert(struct dynamic_string_array *dsa, int string_size) {
	if(dsa->count >= dsa->allocated) {
		dsa->allocated *= 2;
		dsa->strings = realloc(dsa->strings, dsa->allocated * sizeof(char *));
	}
		
	char *string = calloc(string_size + 1, sizeof(char));
	dsa->strings[dsa->count++] = string;

	return string;
}

void dynamic_string_array_free(struct dynamic_string_array *dsa) {
	if(dsa != NULL) {
		int i = 0;
		for(i = 0; i < dsa->count; ++i)
			if(dsa->strings[i] != NULL)
				free(dsa->strings[i]);
		
		if(dsa->strings != NULL)
			free(dsa->strings);

		free(dsa);
	}
}