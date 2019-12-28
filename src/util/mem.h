#pragma once
#include <stdlib.h>

void *memAlloc(size_t size);
void *memRealloc(void *mem, size_t size);
void memFree(void const *mem);
int memLeaks();
