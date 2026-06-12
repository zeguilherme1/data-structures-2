#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "header.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../constants.h"
#include "../utils/string_utils.h"
#include "record.h"
#include "record.h"

Record *new_record()
{

	Record *new_record = (Record *)malloc(sizeof(Record));

	if (new_record == NULL)
	{
		return NULL;
	}

	new_record->removed = FALSE;
	new_record->next_record = -1;
	new_record->station_code = -1;
	new_record->line_code = -1;
	new_record->next_station_code = -1;
	new_record->next_station_distance = -1;
	new_record->line_integration_code = -1;
	new_record->station_integration_code = -1;
	new_record->station_code = -1;
	new_record->station_name_size = 0;
	new_record->station_name = NULL;
	new_record->line_name_size = 0;
	new_record->line_name = NULL;

	return new_record;
}

void print_record(Record *bin_record)
{
	print_int(bin_record->station_code);
	print_string(bin_record->station_name, bin_record->station_name_size);
	print_int(bin_record->line_code);
	print_string(bin_record->line_name, bin_record->line_name_size);
	print_int(bin_record->next_station_code);
	print_int(bin_record->next_station_distance);
	print_int(bin_record->line_integration_code);
	print_int(bin_record->station_integration_code);
	printf("\n");
}

int matches_record_criteria(Record *rec, Search_criteria *criteria, int num_fields)
{
    for (int i = 0; i < num_fields; i++)
    {
        int match = -1;

        if (strcmp(criteria[i].field_name, "nomeEstacao") == 0)
            match = matches_string(criteria[i].field_value, rec->station_name, rec->station_name_size);

        else if (strcmp(criteria[i].field_name, "nomeLinha") == 0)
            match = matches_string(criteria[i].field_value, rec->line_name, rec->line_name_size);

        else if (strcmp(criteria[i].field_name, "codEstacao") == 0)
            match = matches_integer(criteria[i].field_value, rec->station_code);

        else if (strcmp(criteria[i].field_name, "codLinha") == 0)
            match = matches_integer(criteria[i].field_value, rec->line_code);

        else if (strcmp(criteria[i].field_name, "codProxEstacao") == 0)
            match = matches_integer(criteria[i].field_value, rec->next_station_code);

        else if (strcmp(criteria[i].field_name, "distProxEstacao") == 0)
            match = matches_integer(criteria[i].field_value, rec->next_station_distance);

        else if (strcmp(criteria[i].field_name, "codLinhaIntegra") == 0)
            match = matches_integer(criteria[i].field_value, rec->line_integration_code);

        else if (strcmp(criteria[i].field_name, "codEstIntegra") == 0)
            match = matches_integer(criteria[i].field_value, rec->station_integration_code);

        if (match != 0)
            return -1;
    }

    return 0;
}

void free_record(Record **temp_record)
{
	if (!temp_record)
		return;

	if ((*temp_record)->station_name)
	{
		free((*temp_record)->station_name);
		(*temp_record)->station_name = NULL;
	}

	if ((*temp_record)->line_name)
	{
		free((*temp_record)->line_name);
		(*temp_record)->line_name = NULL;
	}

	free(*temp_record);

	*temp_record = NULL;
}
