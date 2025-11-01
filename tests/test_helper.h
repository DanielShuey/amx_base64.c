#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
	char *data;
	int len;
	bool exists;
} file;

static file readfile(const char *filename)
{
	char *buffer = 0;
	long length;
	FILE *f = fopen(filename, "rb");

	if (f) {
		fseek(f, 0, SEEK_END);
		length = ftell(f);
		fseek(f, 0, SEEK_SET);
		buffer = malloc(length);
		if (buffer) {
			fread(buffer, 1, length, f);
		}
		fclose(f);
	}

	file result;
	if (buffer) {
		result.data   = buffer;
		result.len    = length;
		result.exists = true;
		return result;
	} else {
		return result;
	}
}
