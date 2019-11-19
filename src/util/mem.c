#include <stdlib.h>
#include "log.h"
#include "mem.h"


int leakCounter = 0;

void *memAlloc(size_t size) {
	leakCounter++;
	logMsg(LOG_INFO, "Alloc");
	return malloc(size);
}

void *memRealloc(void *mem, size_t size) {
	if (mem == NULL) {
		leakCounter++;
	}
	logMsg(LOG_INFO, "Realloc");
	return realloc(mem, size);
}

void memFree(void *mem) {
	leakCounter--;
	free(mem);
	logMsg(LOG_INFO, "Free");
}

int memLeaks() {
	return leakCounter;
}
