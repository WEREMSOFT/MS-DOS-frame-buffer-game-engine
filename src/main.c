
#include "program/core/array_t/array_t.h"

int main(void)
{
	array_t array = array_create(10, sizeof(char));

	for(char i = 'a'; i < 'z'; i++)
	{
		array.append_element(&array, &i);
	}

	printf("%s\n", array.data);

	for(char i = 'A'; i < 'Z'; i++)
	{
		array.insert_element_at(&array, &i, 10);
	}

	printf("%s\n", array.data);

	typedef struct sarasa_t
	{
		char a, b, c;
	}sarasa_t;

	array_t array_s = array_create(10, sizeof(sarasa_t));

	for(int i = 'a'; i <= 'z'; i++)
	{
		sarasa_t sarasa = {.a = i, .b = i + 1, .c = i + 2 };
		array_s.append_element(&array_s, &sarasa);
	}

	for(int i = 0; i < array_s.length; i++)
	{
		sarasa_t* sarasa = array_s.get_element_at(&array_s, i);
		printf("%c %c %c\n", sarasa->a, sarasa->b, sarasa->c);
	}

	free(array.data);
	free(array_s.data);
	
	return 0;
}