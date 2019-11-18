#include <stdlib.h>
#include "mem.h"


int leakCounter = 0;

void *memAlloc(size_t size) {
	leakCounter++;
	return malloc(size);
}

void memFree(void *mem) {
	leakCounter--;
	free(mem);
}

int memLeaks() {
	return leakCounter;
}
