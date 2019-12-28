#pragma once
#include <stdlib.h>

#define weak 
#define strong

void strong *memAlloc(size_t size);
void strong *memRealloc(void strong *mem, size_t size);
void memFree(void const strong *mem);
int memLeaks();
