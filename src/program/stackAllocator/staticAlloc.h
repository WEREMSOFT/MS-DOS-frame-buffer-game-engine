#ifndef __STATIC_ALLOC_H__
#define __STATIC_ALLOC_H__
#include "stack_allocator.h"
#include <stdlib.h>

void *allocStatic(size_t size);
void *reallocStatic(void *this, size_t size);
void freeStatic(void *this);
void staticAllocatorDestroy();
#endif

#ifdef __STATIC_ALLOC_IMPLEMENTATION__
#undef __STATIC_ALLOC_IMPLEMENTATION__

static StackAllocator *stackAllocator;

void staticAllocatorInit()
{
    stackAllocator = stackAllocatorCreate(4384000);
}

void *allocStatic(size_t size)
{
    void *returnValue = stackAllocatorAlloc(stackAllocator, size);
    return returnValue;
}
void *reallocStatic(void *this, size_t size)
{
    void *returnValue = stackAllocatorRealloc(stackAllocator, this, size);
    return returnValue;
}

void freeStatic(void *this)
{
    if (this == NULL)
        return;
    stackAllocatorFree(stackAllocator);
}

void staticAllocatorDestroy()
{
    free(stackAllocator);
}

#endif