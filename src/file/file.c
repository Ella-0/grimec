#include <stdio.h>
#include "../util/mem.h"

char const strong *readFile(char const weak *path) {
	FILE strong *f = fopen(path, "rb");
	fseek(f, 0, SEEK_END);
	long fsize = ftell(f);
	fseek(f, 0, SEEK_SET);

	char strong *string = memAlloc(fsize + 1);
	fread(string, 1, fsize, f);
	fclose(f);

	string[fsize] = '\0';
	return string;
}
