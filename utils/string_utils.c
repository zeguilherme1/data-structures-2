#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int integer_or_null(char *str)
{
    if (str == NULL)
        return -1;

    // removes \n e \r
    str[strcspn(str, "\r\n")] = '\0';

    // empty string
    if (str[0] == '\0')
        return -1;

    return atoi(str);
}


void print_int(int value)
{
	/*
		Prints an integer field, replacing -1 with NULL representation

		Args:
			(int) value: integer to print

		Return:
			void
	*/

	if (value == -1)
		printf("NULO ");
	else
		printf("%d ", value);
}

void print_string(char *string, int size)
{
	/*
		Prints a string field with given size

		Args:
			(char*) string: string to print
			(int) size: string size

		Return:
			void
	*/

	if (size == 0)
		printf("NULO ");
	else
	{
		printf("%.*s ", size, string);
	}
}

int matches_string(char *criteria_value, char *field, int field_size)
{	
	
	if (strcmp(criteria_value, "NULO") == 0)
	{
		if (field_size == 0) return 0;
		else return -1;
	}
	if (field == NULL) return -1;

    if (strcmp(criteria_value, field) != 0) return -1;
	
	return 0;
}

void clean_string(char *str)
{
    if (!str)
        return;
    str[strcspn(str, "\r\n")] = '\0';
}

int matches_integer(char *criteria_value, int record_field)
{
	/*
		Compares a string field with a search criteria

		Args:
			(char*) criteria_value: value to compare
			(char*) field: record field
			(int) field_size: size of field

		Return:
			0 if match, -1 otherwise
	*/

	if (strcmp(criteria_value, "NULO") == 0)
	{
		if (record_field == -1)
			return 0;
		else
			return -1;
	}
	else
	{
		int integer_field = atoi(criteria_value);
		if (integer_field != record_field)
			return -1;
		else
			return 0;
	}
}