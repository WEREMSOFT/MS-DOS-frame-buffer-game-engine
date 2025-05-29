#ifndef __ARRAY_T_H__
#define __ARRAY_T_H__

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef struct array_t
{
    int length;
	int capacity;
    int elementSize;
    void* data;
	void (*append_element)(array_t* that, void *element);
	void (*insert_element_at)(array_t* that, void *element, int index);
	void (*concatenate)(array_t *that, array_t *source);
	void* (*get_element_at)(array_t *that, int index);	
} array_t;


array_t array_create(int initialCapacity, size_t elementSize);
array_t *array_create_from_c_array(void *c_array, size_t elementSize, int length);
void array_append_element(array_t *that, void *element);
void array_insert_element_at(array_t *that, void *element, int index);
void array_concatenate(array_t *that, array_t *source);
void *array_get_element_at(array_t *that, int index);

#ifndef ARRAY_MALLOC
#define ARRAY_MALLOC malloc
#endif

#ifndef ARRAY_REALLOC
#define ARRAY_REALLOC realloc
#endif

#define __ARRAY_T_IMPLEMENTATION__

#ifdef __ARRAY_T_IMPLEMENTATION__
#undef __ARRAY_T_IMPLEMENTATION__

array_t array_create(int initialCapacity, size_t elementSize)
{
    size_t size = elementSize * initialCapacity;
    array_t array = {
		.length = 0,
		.capacity = initialCapacity,
		.elementSize = elementSize
	};
	
	array.data =  ARRAY_MALLOC(size);

    if (!array.data)
    {
        printf("Error allocation memory for Universalarray_t %s::%d\n", __FILE__, __LINE__);
        exit(-1);
    }

	memset(array.data, 0, size);
    return array;
}

#endif
#endif