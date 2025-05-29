#ifndef __UNIVERSAL_ARRAY_H__
#define __UNIVERSAL_ARRAY_H__

#include "array_header.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef struct Array
{
    ArrayHeader header;
    char data[1];
} Array;

Array *array_create(int initialCapacity, size_t elementSize);
Array *array_create_from_c_array(void *c_array, size_t elementSize, int length);
void array_append_element(Array **this, void *element);
void array_insert_element_at(Array **this, void *element, int index);
void array_concatenate(Array **this, Array *source);
void *array_get_element_at(Array *this, int index);
#endif

#ifndef ARRAY_MALLOC
#define ARRAY_MALLOC malloc
#endif

#ifndef ARRAY_REALLOC
#define ARRAY_REALLOC realloc
#endif

#ifdef __UNIVERSAL_ARRAY_IMPLEMENTATION__
#undef __UNIVERSAL_ARRAY_IMPLEMENTATION__

Array *array_create(int initialCapacity, size_t elementSize)
{
    size_t size = elementSize * initialCapacity + sizeof(ArrayHeader);
    Array *array = (Array *)ARRAY_MALLOC(size);

    if (!array)
    {
        printf("Error allocation memory for UniversalArray %s::%d\n", __FILE__, __LINE__);
        exit(-1);
    }

    memset(array, 0, size);

    array->header.capacity = initialCapacity;
    array->header.elementSize = elementSize;
    array->header.length = 0;
    return array;
}

Array *array_create_from_c_array(void *c_array, size_t elementSize, int length)
{
    Array *array = array_create(length, elementSize);
    array->header.length = length;
    memcpy(array->data, c_array, length * elementSize);
    return array;
}

void array_append_element(Array **this, void *element)
{
    if ((*this)->header.length + 2 >= (*this)->header.capacity)
    {
        int size = (*this)->header.capacity * (*this)->header.elementSize * 2 + sizeof(ArrayHeader);
        Array *new_pointer = ARRAY_REALLOC(*this, size);
        if (new_pointer == NULL)
        {
            printf("Error reallocating array\n");
            exit(-1);
        }
        		
		*this = new_pointer;
		memset(&new_pointer->data[new_pointer->header.elementSize * new_pointer->header.length], 0, (new_pointer->header.capacity - new_pointer->header.length - 1) * new_pointer->header.elementSize);
		(*this)->header.capacity *= 2;
    }

    memcpy(&(**this).data[(*this)->header.elementSize * (*this)->header.length], element, (**this).header.elementSize);
    (*this)->header.length++;
}

void array_insert_element_at(Array **that, void *element, int index)
{
    if ((*that)->header.length + 2 >= (*that)->header.capacity)
    {
        int size = (*that)->header.capacity * (*that)->header.elementSize * 2 + sizeof(ArrayHeader);
        Array *new_pointer = ARRAY_REALLOC(*that, size);
        if (new_pointer == NULL)
        {
            printf("Error reallocating array\n");
            exit(-1);
        }
		
		
		new_pointer->header.capacity *= 2;
		memset(&new_pointer->data[new_pointer->header.elementSize * new_pointer->header.length], 0, (new_pointer->header.capacity - new_pointer->header.length - 1) * new_pointer->header.elementSize);

		*that = new_pointer;
    }

    memmove(&(**that).data[(*that)->header.elementSize * index + 1], 
	&(**that).data[(*that)->header.elementSize * index]
	, (**that).header.elementSize * ((**that).header.length) - index);

    memmove(&(**that).data[(*that)->header.elementSize * index], element, (**that).header.elementSize);
    (*that)->header.length++;
}

void array_concatenate(Array **this, Array *source)
{
    if ((*this)->header.elementSize != source->header.elementSize)
    {
        printf("Error: Arrays holds elements of different dizes. %s::%d\n", __FILE__, __LINE__);
        exit(-1);
    }
    for (int i = 0; i < source->header.length; i++)
    {
        array_append_element(this, &source->data[i]);
    }
}

void *array_get_element_at(Array *this, int index)
{
    if (index < this->header.length)
    {
        return &this->data[index * this->header.elementSize];
    }
    return NULL;
}
#endif
