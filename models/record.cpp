#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "header.h"
#include "../constants.h"
#include "../utils/utils.h"
#include "record.h"

// * Memory management

Record *new_record()
{
	// Allocate memory for the main record structure
	Record *new_record = (Record *)malloc(sizeof(Record));

	// Safety check in case memory allocation fails
	if (new_record == NULL)
	{
		return NULL;
	}

	// Initialize base fields to default safety values
	new_record->removed = FALSE;
	new_record->next_record = -1;
	new_record->station_code = -1;
	new_record->line_code = -1;
	new_record->next_station_code = -1;
	new_record->next_station_distance = -1;
	new_record->line_integration_code = -1;
	new_record->station_integration_code = -1;

	// Initialize variable-length string fields to 0 and NULL
	new_record->station_name_size = 0;
	new_record->station_name = NULL;
	new_record->line_name_size = 0;
	new_record->line_name = NULL;

	return new_record;
}

void free_record(Record **temp_record)
{
	// Safety check to avoid dereferencing a NULL pointer
	if (!temp_record || !*temp_record)
		return;

	// Check and free the station name string if it was dynamically allocated
	if ((*temp_record)->station_name)
	{
		free((*temp_record)->station_name);
		(*temp_record)->station_name = NULL;
	}

	// Check and free the line name string if it was dynamically allocated
	if ((*temp_record)->line_name)
	{
		free((*temp_record)->line_name);
		(*temp_record)->line_name = NULL;
	}

	// Free the main record structure
	free(*temp_record);

	// Nullify the original pointer to avoid dangling references
	*temp_record = NULL;
}

// * Display formatting

void print_record(Record *bin_record)
{
	// Print fixed and variable fields in the expected output order using helper functions
	print_int(bin_record->station_code);
	print_string(bin_record->station_name, bin_record->station_name_size);
	print_int(bin_record->line_code);
	print_string(bin_record->line_name, bin_record->line_name_size);
	print_int(bin_record->next_station_code);
	print_int(bin_record->next_station_distance);
	print_int(bin_record->line_integration_code);
	print_int(bin_record->station_integration_code);

	// Append a newline after printing all fields for the given record
	printf("\n");
}

// * File operations

Record *read_rrn_record(FILE *bin_file, int rrn)
{
	// Calculate precise byte offset using standard equation: Header Size + (RRN * Record Size)
	int byte_offset = (HEADER_SIZE + (RECORD_SIZE * rrn));

	// Seek directly to the target record's position in the binary file
	fseek(bin_file, byte_offset, SEEK_SET);

	// Allocate memory to hold the data we are about to read
	Record *find_record = (Record *)malloc(sizeof(Record));
	if (find_record == NULL)
		return NULL;

	// Read the first byte to check if the record is logically removed
	char removido;
	fread(&removido, sizeof(char), 1, bin_file);
	find_record->removed = removido;

	// If the record is marked as removed, abort reading and return NULL
	if (removido == TRUE)
	{
		free(find_record);
		return NULL;
	}

	// Read all fixed-size integer fields sequentially
	fread(&find_record->next_record, sizeof(int), 1, bin_file);
	fread(&find_record->station_code, sizeof(int), 1, bin_file);
	fread(&find_record->line_code, sizeof(int), 1, bin_file);
	fread(&find_record->next_station_code, sizeof(int), 1, bin_file);
	fread(&find_record->next_station_distance, sizeof(int), 1, bin_file);
	fread(&find_record->line_integration_code, sizeof(int), 1, bin_file);
	fread(&find_record->station_integration_code, sizeof(int), 1, bin_file);

	// Read the size of the station name string
	fread(&find_record->station_name_size, sizeof(int), 1, bin_file);

	// Allocate memory and read the actual string if the size is greater than 0
	if (find_record->station_name_size > 0)
	{
		find_record->station_name = (char*)malloc(find_record->station_name_size + 1);
		fread(find_record->station_name, sizeof(char), find_record->station_name_size, bin_file);
		find_record->station_name[find_record->station_name_size] = '\0'; // Null-terminate
	}
	else
	{
		find_record->station_name = NULL;
	}

	// Read the size of the line name string
	fread(&find_record->line_name_size, sizeof(int), 1, bin_file);

	// Allocate memory and read the actual string if the size is greater than 0
	if (find_record->line_name_size > 0)
	{
		find_record->line_name = (char*)malloc(find_record->line_name_size + 1);
		fread(find_record->line_name, sizeof(char), find_record->line_name_size, bin_file);
		find_record->line_name[find_record->line_name_size] = '\0'; // Null-terminate
	}
	else
	{
		find_record->line_name = NULL;
	}

	return find_record;
}