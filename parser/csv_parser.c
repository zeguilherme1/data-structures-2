#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../models/record.h"

#include "../utils/string_utils.h"

#include "tokenizer.h"
#include "csv_parser.h"

Record *tokenize_record(char *buffer)
{
	Record *temp_record = (Record *)malloc(sizeof(Record));
	if (temp_record == NULL)
		return NULL;

	char *token;

	token = meu_strtok(&buffer, ",");
	temp_record->station_code = integer_or_null(token);

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
		token[strcspn(token, "\r\n")] = '\0';
		temp_record->line_name = strdup(token);
		temp_record->line_name_size = strlen(token);
	}
	else
	{
		temp_record->line_name = strdup("");
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

	return temp_record;
}