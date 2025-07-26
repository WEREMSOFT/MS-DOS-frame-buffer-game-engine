#ifndef __MEMORY_H__
#define __MEMORY_H__
#include <stdlib.h>

void* cmalloc(size_t size);

#ifndef MALLOC
#define MALLOC cmalloc
#endif

#ifndef REALLOC
#define REALLOC realloc
#endif

#ifndef FREE
#define FREE free
#endif

#endif

