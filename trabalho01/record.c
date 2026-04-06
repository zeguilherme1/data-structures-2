#include <stdio.h>
#include <string.h>
#include "header.h"
#include "utils.h"
#include "record.h"

Record *new_record()
{
	/*
		This function creates a new record and set up initial values

		Args:
			No args

		Return:
			new_record: The generated record

	*/

	Record *new_record = (Record *)malloc(sizeof(Record));

	if (new_record == NULL)
	{
		return NULL;
	}

	new_record->removed = '0';
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

Record *tokenize_record(char *buffer)
{
	Record *temp_record = (Record *)malloc(sizeof(Record));

	if (temp_record == NULL)
	{
		return NULL;
	}

	char *token = strtok(buffer, ",");
	temp_record->station_code = integer_or_null(token);

	token = strtok(NULL, ",");
	token[strcspn(token, "\r\n")] = '\0';
	temp_record->station_name = strdup(token);
	temp_record->station_name_size = strlen(token);

	token = strtok(NULL, ",");
	temp_record->line_code = integer_or_null(token);
	token = strtok(NULL, ",");
	token[strcspn(token, "\r\n")] = '\0';
	temp_record->line_name = strdup(token);
	temp_record->line_name_size = strlen(token);

	token = strtok(NULL, ",");
	temp_record->next_station_code = integer_or_null(token);

	token = strtok(NULL, ",");
	temp_record->next_station_distance = integer_or_null(token);

	token = strtok(NULL, ",");
	temp_record->line_integration_code = integer_or_null(token);

	token = strtok(NULL, ",");
	temp_record->station_integration_code = integer_or_null(token);

	return temp_record;
}

void save_record_to_bin(FILE *bin_filename, Record *new_record)
{
	long record_begin = ftell(bin_filename);

	save_record(bin_filename, new_record);

	fwrite(&new_record->station_name_size, sizeof(int), 1, bin_filename);

	if (new_record->station_name_size > 0)
	{
		fwrite(new_record->station_name, sizeof(char), new_record->station_name_size, bin_filename);
	}

	fwrite(&new_record->line_name_size, sizeof(int), 1, bin_filename);
	if (new_record->line_name_size > 0)
	{
		fwrite(new_record->line_name, sizeof(char), new_record->line_name_size, bin_filename);
	}

	long record_end = ftell(bin_filename);

	int remain_bytes = RECORD_SIZE - (record_end - record_begin);

	if (remain_bytes > 0)
	{
		char empty = '$';
		for (int i = 0; i < remain_bytes; i++)
			fwrite(&empty, sizeof(char), 1, bin_filename);
	}
}

void save_record(FILE *bin_filename, Record *new_record)
{

	fwrite(&new_record->removed, sizeof(char), 1, bin_filename);
	fwrite(&new_record->next_record, sizeof(int), 1, bin_filename);
	fwrite(&new_record->station_code, sizeof(int), 1, bin_filename);
	fwrite(&new_record->line_code, sizeof(int), 1, bin_filename);
	fwrite(&new_record->next_station_code, sizeof(int), 1, bin_filename);
	fwrite(&new_record->next_station_distance, sizeof(int), 1, bin_filename);
	fwrite(&new_record->line_integration_code, sizeof(int), 1, bin_filename);
	fwrite(&new_record->station_integration_code, sizeof(int), 1, bin_filename);
}

int write_record(char *filename, Record *new_record)
{
	FILE *file = fopen(filename, READ_BINARY_MODE);

	// todo: implement the complete function

	fclose(file);
	return SUCCESS;
}

int read_record(FILE *bin_file, Record *bin_record)
{
	if (bin_file == NULL || bin_record == NULL) return NO_DATA_ERROR;

	int verify = 0;

	verify += fread(&bin_record->removed, sizeof(char), 1, bin_file);
	verify += fread(&bin_record->next_record, sizeof(int), 1, bin_file);
	verify += fread(&bin_record->station_code, sizeof(int), 1, bin_file);
	verify += fread(&bin_record->line_code, sizeof(int), 1, bin_file);
	verify += fread(&bin_record->next_station_code, sizeof(int), 1, bin_file);
	verify += fread(&bin_record->next_station_distance, sizeof(int), 1, bin_file);
	verify += fread(&bin_record->line_integration_code, sizeof(int), 1, bin_file);
	verify += fread(&bin_record->station_integration_code, sizeof(int), 1, bin_file);


	verify += fread(&bin_record->station_name_size, sizeof(int), 1, bin_file);
	if(bin_record->station_name_size > 0) {
		bin_record->station_name = malloc(bin_record->station_name_size + 1);
		if (bin_record->station_name == NULL) return NO_DATA_ERROR;

		verify += fread(bin_record->station_name, sizeof(char), bin_record->station_name_size, bin_file);

		bin_record->station_name[bin_record->station_name_size] = '\0';
	} else {
		bin_record->station_name = NULL;
	}

	verify += fread(&bin_record->line_name_size, sizeof(int), 1, bin_file);
	if(bin_record->line_name_size > 0){
		bin_record->line_name = malloc(bin_record->line_name_size + 1);
		if (bin_record->line_name == NULL) return NO_DATA_ERROR;
		verify += fread(bin_record->line_name, sizeof(char), bin_record->line_name_size, bin_file);

		bin_record->line_name[bin_record->line_name_size] = '\0';
	}else {
		bin_record->line_name = NULL;
	}

	char trash[43];
	int trash_size = 43 - bin_record->station_name_size - bin_record->line_name_size;

	if(trash_size > 0) fread(trash, sizeof(char), trash_size, bin_file);

	if(verify == 10 + bin_record->station_name_size + bin_record->line_name_size) return 0;

	return -1;
}

void print_int(int value)
{
	if (value == -1)
		printf("NULO ");
	else
		printf("%d ", value);
}

void print_string(char *string, int size)
{
	if (size == 0)
		printf("NULO ");
	else
	{
		printf("%.*s ", size, string);
	}
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

int matches_string(char *criteria_value, char *field, int field_size){
	if (strcmp(criteria_value, "NULO") == 0){
		if (field_size == 0) return 0;
		else return -1;
	} else{
		if (strcmp(criteria_value, field) != 0) return -1;
		else return 0;
	}
}

int matches_integer(char *criteria_value, int record_field){
	if (strcmp(criteria_value, "NULO") == 0){
		if (record_field == -1) return 0;
		else return -1;
	} else{
		int integer_field = atoi(criteria_value);
		if (integer_field != record_field) return -1;
		else return 0;
	}
}

int matches_record_criteria(Record *bin_record, Search_criteria *criteria, int num_fields){
	/*

		This function confirms if the record was successfully compared with the criteria

		Args:

			(Record*) bin_record: the record struct to be compared

			(Search_criteria*) criteria: the criteria struct that will be comparison parameter

			int num_fields: the number of fields to be compared

		Return:

			0 for success and -1 for fail

	*/

	for (int i = 0; i < num_fields; i++){
		int ret_match;

		if (strcmp(criteria[i].field_name, "nomeEstacao") == 0){
			ret_match = matches_string(criteria[i].field_value, bin_record->station_name, bin_record->station_name_size);
			if (ret_match) return -1;
		} else if (strcmp(criteria[i].field_name, "nomeLinha") == 0){
			ret_match = matches_string(criteria[i].field_value, bin_record->line_name, bin_record->line_name_size);
			if (ret_match) return -1;
		} else if (strcmp(criteria[i].field_name, "codEstacao") == 0){
			ret_match = matches_integer(criteria[i].field_value, bin_record->station_code);
			if (ret_match) return -1;
		} else if (strcmp(criteria[i].field_name, "codLinha") == 0){
			ret_match = matches_integer(criteria[i].field_value, bin_record->line_code);
			if (ret_match) return -1;
		} else if (strcmp(criteria[i].field_name, "codProxEstacao") == 0){
			ret_match = matches_integer(criteria[i].field_value, bin_record->next_station_code);
			if (ret_match) return -1;
		} else if (strcmp(criteria[i].field_name, "distProxEstacao") == 0){
			ret_match = matches_integer(criteria[i].field_value, bin_record->next_station_distance);
			if (ret_match) return -1;
		} else if (strcmp(criteria[i].field_name, "codLinhaIntegra") == 0){
			ret_match = matches_integer(criteria[i].field_value, bin_record->line_integration_code);
			if (ret_match) return -1;
		} else if (strcmp(criteria[i].field_name, "codEstIntegra") == 0){
			ret_match = matches_integer(criteria[i].field_value, bin_record->station_integration_code);
			if (ret_match) return -1;
		} 
	}

	return SUCCESS;
}

Record *read_rrn_record(FILE *bin_file, int rrn)
{
	// calcula o byteoffset
	// move o ponteiro usando fseek
	// le os campos fixos do registro
	// le os campos variaveis

	int byte_offset = (HEADER_SIZE + (RECORD_SIZE * rrn));
	fseek(bin_file, byte_offset, SEEK_SET);

	Record *find_record = (Record *)malloc(sizeof(Record));
	if (find_record == NULL)
		return NULL;

	char removido;
	fread(&removido, sizeof(char), 1, bin_file);

	find_record->removed = removido;

	if (removido == '1')
	{
		free(find_record);
		return NULL;
	}

	fread(&find_record->next_record, sizeof(int), 1, bin_file);
	fread(&find_record->station_code, sizeof(int), 1, bin_file);
	fread(&find_record->line_code, sizeof(int), 1, bin_file);
	fread(&find_record->next_station_code, sizeof(int), 1, bin_file);
	fread(&find_record->next_station_distance, sizeof(int), 1, bin_file);
	fread(&find_record->line_integration_code, sizeof(int), 1, bin_file);
	fread(&find_record->station_integration_code, sizeof(int), 1, bin_file);

	fread(&find_record->station_name_size, sizeof(int), 1, bin_file);
	if (find_record->station_name_size > 0)
	{
		find_record->station_name = (char *)calloc(find_record->station_name_size, sizeof(char));
	}
	fread(find_record->station_name, sizeof(char), find_record->station_name_size, bin_file);

	fread(&find_record->line_name_size, sizeof(int), 1, bin_file);
	if (find_record->line_name_size > 0)
	{
		find_record->line_name = (char *)calloc(find_record->line_name_size, sizeof(char));
	}
	fread(find_record->line_name, sizeof(char), find_record->line_name_size, bin_file);

	return find_record;
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

int search_rrn()
{
	char bin_filename[100];
	int rrn;

	scanf("%s %d", bin_filename, &rrn);

	FILE *bin_file = fopen(bin_filename, READ_BINARY_MODE);

	Header *bin_header = read_binary_header(bin_file);

	if (bin_header == NULL)
		return MALLOC_ERROR;

	Record *result_record = read_rrn_record(bin_file, rrn);

	print_record(result_record);
}