#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../models/record.h"
#include "../utils/utils.h"

#include "tokenizer.h"

// * Record tokenization and string splitting

Record *tokenize_record(char *buffer)
{
	// Allocate memory for the new record structure
	Record *temp_record = (Record *)malloc(sizeof(Record));
	if (temp_record == NULL)
		return NULL; // Memory allocation failed

	char *token;

	// Extract the first token (station_code) using custom tokenizer
	token = meu_strtok(&buffer, ",");
	temp_record->station_code = integer_or_null(token); // Parse integer or handle missing data

	// Extract the second token (station_name)
	token = meu_strtok(&buffer, ",");
	if (token)
	{
		// Remove any trailing newlines or carriage returns
		token[strcspn(token, "\r\n")] = '\0';
		// Duplicate the string safely into the record
		temp_record->station_name = strdup(token);
		temp_record->station_name_size = strlen(token);
	}
	else
	{
		// Handle missing string by assigning an empty string and size 0
		temp_record->station_name = strdup("");
		temp_record->station_name_size = 0;
	}

	// Extract the third token (line_code)
	token = meu_strtok(&buffer, ",");
	temp_record->line_code = integer_or_null(token);

	// Extract the fourth token (line_name)
	token = meu_strtok(&buffer, ",");
	if (token)
	{
		// Remove newline characters before saving
		token[strcspn(token, "\r\n")] = '\0';
		temp_record->line_name = strdup(token);
		temp_record->line_name_size = strlen(token);
	}
	else
	{
		// Fallback for empty line name field
		temp_record->line_name = strdup("");
		temp_record->line_name_size = 0;
	}

	// Extract the fifth token (next_station_code)
	token = meu_strtok(&buffer, ",");
	temp_record->next_station_code = integer_or_null(token);

	// Extract the sixth token (next_station_distance)
	token = meu_strtok(&buffer, ",");
	temp_record->next_station_distance = integer_or_null(token);

	// Extract the seventh token (line_integration_code)
	token = meu_strtok(&buffer, ",");
	temp_record->line_integration_code = integer_or_null(token);

	// Extract the eighth and final token (station_integration_code)
	token = meu_strtok(&buffer, ",");
	temp_record->station_integration_code = integer_or_null(token);

	return temp_record; // Return the fully populated record
}

char *meu_strtok(char **buffer, const char *delimiter)
{
	// Safety check: if buffer is null or points to a null string, there's nothing to tokenize
	if (buffer == NULL || *buffer == NULL)
		return NULL; // No more tokens available

	char *start = *buffer;
	char *delimiter_position;

	// Search for the first occurrence of any character from the delimiter string
	if ((delimiter_position = strpbrk(start, delimiter)) != NULL)
	{
		*delimiter_position = '\0';		  // Replace the delimiter with a null terminator to isolate the token
		*buffer = delimiter_position + 1; // Move the original buffer pointer to the character right after the delimiter
	}
	else
	{
		// No more delimiters found, meaning this is the last token in the string
		*buffer = NULL;
	}

	return start; // Return the isolated token
}