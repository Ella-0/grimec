#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "log.h"
#include "mem.h"

static int leakCount = 0;
static size_t leakSize = 0;

static const int METADATA_SIZE = 0; // WORKS TODO fix memleak

static const int PADDING_SIZE = 0;

void strong *memAlloc(size_t size) {
	void *raw = malloc(METADATA_SIZE + size + PADDING_SIZE);
	//memset(raw, 0, METADATA_SIZE);
	return raw + METADATA_SIZE;
}

void strong *memRealloc(void strong *mem, size_t size) {
	if (mem == NULL) {
		return memAlloc(size);
	}
	void strong *raw = (void *) mem - METADATA_SIZE;
	for (unsigned int i = 0; i < METADATA_SIZE; i++) {
		int b = (int) *((char *) raw + i);
		if (b != 0) {
			logMsg(LOG_INFO, 0, "%u, %u", i, b);
			exit(EXIT_FAILURE);
		}
	}
	return realloc(raw, METADATA_SIZE + size + PADDING_SIZE) + METADATA_SIZE;
}

void memFree(void const strong *mem) {
	if (mem == NULL) {
		return;
	}
	void strong *raw = (void *) mem - METADATA_SIZE;
	for (unsigned int i = 0; i < METADATA_SIZE; i++) {
		int b = (int) *((char *) raw + i);
		if (b != 0) {
			logMsg(LOG_INFO, 0, "%u, %u", i, b);
			exit(EXIT_FAILURE);
		}
	}
//	free(raw);
}


void strong *xmemAlloc(size_t size) {
	logMsg(LOG_INFO, 0, "Alloc %u", size);
	
	size_t strong *raw = (size_t strong *) malloc(METADATA_SIZE * sizeof(size_t) + size + PADDING_SIZE);
	if (raw == NULL) {
		logMsg(LOG_ERROR, 4, "Failed To Allocate Memory!");
		exit(EXIT_FAILURE);
	}

	leakCount++;
	leakSize += size;

	*raw = size;
	void strong *ret = (void strong *) (raw + METADATA_SIZE);
	//memset(ret, 0xff, size);
	return ret;
}

void strong *xmemRealloc(void strong *mem, size_t size) {
	if (mem == NULL) {
		return memAlloc(size);
	}
	
	size_t strong *raw = ((size_t strong *) mem) - METADATA_SIZE;
	size_t oldSize = *raw;	
	
	logMsg(LOG_INFO, 0, "Realloc %u -> %u", oldSize, size);
	
	leakSize += size - oldSize;

	raw = (size_t strong *) realloc(raw, METADATA_SIZE * sizeof(size_t) + size + PADDING_SIZE);
	if (raw == NULL) {
		logMsg(LOG_ERROR, 4, "Failed To Reallocate Memory!");
		exit(EXIT_FAILURE);
	}

	*raw = size;
	void strong *ret = (void strong *) (raw + METADATA_SIZE);
	if (size >= oldSize) {
		//memset(ret + oldsize, 0xff, size - oldSize);
	}
	return ret;
}

void xmemFree(void const strong *mem) {
	if (mem == NULL) {
		return;
	}
	size_t strong *raw = ((size_t strong *) mem) - METADATA_SIZE;
	size_t oldSize = *raw;
	
	logMsg(LOG_INFO, 0, "Free %u", oldSize);

	leakCount--;
	leakSize -= oldSize;

	free((void *) raw);
}

int memLeaks() {
	return leakCount;
}

char const strong *heapString(char const weak *stringLiteral) {
	char strong *ret = memAlloc(sizeof(char) * (strlen(stringLiteral) + 1));
	strcpy(ret, stringLiteral);
	return ret;
}
