#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "log.h"
#include "mem.h"


static int leakCounter = 0;

void *memAlloc(size_t size) {
	leakCounter++;
	logMsg(LOG_INFO, 0, "Alloc %u", size);
	
	void *ret = malloc(size);
	if (size > 0) {
		if (ret == NULL) {
			logMsg(LOG_ERROR, 4, "Failed To Allocate Memory!");
			exit(EXIT_FAILURE);
		}
		//ret = memset(ret, 0xff, size);
	} else {
		leakCounter--;
	}
	return ret;
	//return malloc(size);
}

void *memRealloc(void *mem, size_t size) {
	logMsg(LOG_INFO, 0, "Realloc");
	bool memNull = mem == NULL;
	void *ret = realloc(mem, size);
	if (size > 0) {
		if (ret == NULL) {
			logMsg(LOG_ERROR, 4, "Failed To Reallocate Memory!");
			exit(EXIT_FAILURE);
		}
		if (memNull) {
			leakCounter++;
			ret = memset(ret, 0xff, size);
		}
	} else {
		leakCounter--;
	}
	return ret;
}

void memFree(void const *mem) {
	leakCounter--;
	free((void *) mem);
	logMsg(LOG_INFO, 0, "Free");
}

int memLeaks() {
	return leakCounter;
}
