#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../models/record.h"
#include "../utils/string_utils.h"

#include "tokenizer.h"


Record *tokenize_record(char *buffer)
{
	Record *temp_record = (Record *)malloc(sizeof(Record));
	if (temp_record == NULL)
		return NULL;

	char *token;

	token = meu_strtok(&buffer, ",");
	temp_record->station_code = integer_or_null(token); //parse int or null

	token = meu_strtok(&buffer, ",");
	if (token)
	{
		token[strcspn(token, "\r\n")] = '\0';
		temp_record->station_name = strdup(token);
		temp_record->station_name_size = strlen(token);
	}
	else
	{
		temp_record->station_name = strdup("");
		temp_record->station_name_size = 0;
	}

	token = meu_strtok(&buffer, ",");
	temp_record->line_code = integer_or_null(token);

	token = meu_strtok(&buffer, ",");
	if (token)
	{
		token[strcspn(token, "\r\n")] = '\0'; //remove newline
		temp_record->line_name = strdup(token);
		temp_record->line_name_size = strlen(token);
	}
	else
	{
		temp_record->line_name = strdup(""); //empty field
		temp_record->line_name_size = 0;
	}

	token = meu_strtok(&buffer, ",");
	temp_record->next_station_code = integer_or_null(token);

	token = meu_strtok(&buffer, ",");
	temp_record->next_station_distance = integer_or_null(token);

	token = meu_strtok(&buffer, ",");
	temp_record->line_integration_code = integer_or_null(token);

	token = meu_strtok(&buffer, ",");
	temp_record->station_integration_code = integer_or_null(token);

	return temp_record; //fully built record
}

char *meu_strtok(char **buffer, const char *delimiter)
{
    if (buffer == NULL || *buffer == NULL)
        return NULL; // no more tokens

    char *start = *buffer;
    char *delimiter_position;

    // find next delimiter
    if ((delimiter_position = strpbrk(start, delimiter)) != NULL)
    {
        *delimiter_position = '\0'; // terminate token
        *buffer = delimiter_position + 1; // move forward
    }
    else
        *buffer = NULL; // last token

    return start;
}
