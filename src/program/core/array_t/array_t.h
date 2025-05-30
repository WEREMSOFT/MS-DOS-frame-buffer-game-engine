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
	void (*append_element)(struct array_t* that, void *element);
	void (*insert_element_at)(struct array_t* that, void *element, int index);
	void* (*get_element_at)(struct array_t that, int index);
	void* (*delete_element_at)(struct array_t *that, int index);
} array_t;


array_t array_create(int initialCapacity, size_t elementSize);
void array_append_element(struct array_t *that, void *element);
void array_insert_element_at(struct array_t *that, void *element, int index);
void *array_get_element_at(array_t that, int index);
void array_delete_element_at(struct array_t *that, int index);


#ifndef ARRAY_MALLOC
#define ARRAY_MALLOC malloc
#endif

#ifndef ARRAY_REALLOC
#define ARRAY_REALLOC realloc
#endif

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
	array.append_element = array_append_element;
	array.insert_element_at = array_insert_element_at;
	array.get_element_at = array_get_element_at;
	array.delete_element_at = array_delete_element_at;
    return array;
}

void array_append_element(array_t *that, void *element)
{
	if (that->length + 1 >= that->capacity)
    {
        int size = that->capacity * that->elementSize * 2;
        void* new_pointer = ARRAY_REALLOC(that->data, size);
        if (new_pointer == NULL)
        {
            printf("Error reallocating array\n");
            exit(-1);
        }
        		
		that->capacity *= 2;
		that->data = new_pointer;
		memset(that->data + that->elementSize * that->length, 0, (that->capacity - that->length - 1) * that->elementSize);
    }

    memmove(that->data + that->elementSize * that->length, element, that->elementSize);
    that->length++;
}

void array_insert_element_at(array_t *that, void *element, int index)
{
    if (that->length + 1 >= that->capacity)
    {
        int size = that->capacity * that->elementSize * 2;
        void *new_pointer = ARRAY_REALLOC(that->data, size);
        if (new_pointer == NULL)
        {
            printf("Error reallocating array\n");
            exit(-1);
        }
				
		that->capacity *= 2;
		that->data = new_pointer;
		memset(that->data + that->elementSize * that->length, 0, (that->capacity - that->length - 1) * that->elementSize);

    }

	that->length++;

    memmove(that->data + that->elementSize * (index + 1), 
			that->data + that->elementSize * index,
			that->elementSize * (that->length - index));

    memmove(that->data + that->elementSize * index, 
			element, that->elementSize);
}

void *array_get_element_at(array_t that, int index)
{
    if (index < that.length)
    {
        return &that.data[index * that.elementSize];
    }
    return NULL;
}

void array_delete_element_at(struct array_t* that, int index)
{
	for(int i = index; i < that->length; i++)
	{
		memmove(that->data + i, that->data + i+1, that->elementSize);
	}
	that->length--;
}

#endif
#endif