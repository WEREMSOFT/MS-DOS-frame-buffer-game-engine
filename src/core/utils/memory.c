#include <stdlib.h>
#include "memory.h"

void* cmalloc(size_t size)
{
	return calloc(size, sizeof(char));
}

