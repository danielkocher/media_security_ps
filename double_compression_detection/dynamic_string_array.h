#ifndef _DYNAMIC_STRING_ARRAY_H_
#define _DYNAMIC_STRING_ARRAY_H_

struct dynamic_string_array {
	int allocated;
	int count;
	char **strings;
};

char *dynamic_string_array_insert(struct dynamic_string_array *dsa, int string_size);
void dynamic_string_array_free(struct dynamic_string_array *dsa);

#endif // _DYNAMIC_STRING_ARRAY_H_