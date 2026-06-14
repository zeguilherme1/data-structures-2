#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "./utils.h"

// *String and formatting utils

int integer_or_null(char *str)
{
	// Check for a null pointer safety
	if (str == NULL)
		return -1;

	// Remove line breaks and carriage returns from the string end
	str[strcspn(str, "\r\n")] = '\0';

	// If the string becomes empty after cleaning, it represents a NULL integer
	if (str[0] == '\0')
		return -1;

	// Convert the valid string to an integer and return
	return atoi(str);
}

void print_int(int value)
{
	// -1 is the designated flag for null/missing integer fields in our system
	if (value == -1)
		printf("NULO ");
	else
		printf("%d ", value); // Print the actual integer with a trailing space
}

void print_string(char *string, int size)
{
	// A string of size 0 means there is no data to be printed
	if (size == 0)
		printf("NULO ");
	else
	{
		// Print exactly 'size' characters from the string pointer
		// This avoids printing garbage data if the string is not null-terminated correctly
		printf("%.*s ", size, string);
	}
}

int matches_string(char *criteria_value, char *field, int field_size)
{
	// Handle the specific text input "NULO" which indicates the user is searching for empty fields
	if (strcmp(criteria_value, "NULO") == 0)
	{
		// If the record's field size is also 0, it's a match
		return (field_size == 0) ? 0 : -1;
	}

	// If the criteria isn't "NULO" but the field is empty or null, it's a mismatch
	if (field_size == 0 || field == NULL)
		return -1;

	// Create a temporary buffer to safely copy and null-terminate the record's field
	char temp[field_size + 1];
	memcpy(temp, field, field_size);
	temp[field_size] = '\0';

	// Compare the extracted string with the user's search criteria
	if (strcmp(criteria_value, temp) != 0)
		return -1; // They don't match

	return 0; // Perfect match
}

void clean_string(char *str)
{
	// Null pointer safety check
	if (!str)
		return;

	// Find the first occurrence of \r or \n and replace it with a null terminator
	str[strcspn(str, "\r\n")] = '\0';
}

int matches_integer(char *criteria_value, int record_field)
{
	// Handle the specific text input "NULO" meaning the user searches for missing integer fields
	if (strcmp(criteria_value, "NULO") == 0)
	{
		// -1 represents missing integers internally
		if (record_field == -1)
			return 0; // Match
		else
			return -1; // Mismatch
	}
	else
	{
		// The user provided a standard number string, convert it to compare
		int integer_field = atoi(criteria_value);

		if (integer_field != record_field)
			return -1; // Numbers are different
		else
			return 0; // Numbers are equal
	}
}

void scan_quote_string(char *str)

{
	char R;

	while ((R = getchar()) != EOF && isspace(R))
		; // ignorar espaços, \r, \n...

	if (R == 'N' || R == 'n')
	{ // campo NULO
		getchar();
		getchar();
		getchar();		 // ignorar o "ULO" de NULO.
		strcpy(str, ""); // copia string vazia
	}

	else if (R == '\"')
	{
		if (scanf("%[^\"]", str) != 1)
		{ // ler até o fechamento das aspas
			strcpy(str, "");
		}
		getchar(); // ignorar aspas fechando
	}

	else if (R != EOF)
	{ // vc tá tentando ler uma string que não tá entre
	  // aspas! Fazer leitura normal %s então, pois deve
	  // ser algum inteiro ou algo assim...
		str[0] = R;
		scanf("%s", &str[1]);
	}

	else
	{ // EOF
		strcpy(str, "");
	}
}