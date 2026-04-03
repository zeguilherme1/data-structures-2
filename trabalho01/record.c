#include <stdio.h>
#include <string.h>
#include "utils.h"
#include "record.h"


Record* new_record(){
    /*
        This function creates a new record and set up initial values

        Args:
            No args

        Return:
            new_record: The generated record
    
    */

Record* new_record = (Record*)malloc(sizeof(Record));

    if(!new_record) {
        return NULL;
    }

    new_record->removed = '0';
    new_record->next_record = -1;
    new_record->station_code = -1;
    new_record->line_code = -1;
    new_record->next_station_code = -1;
	new_record->next_station_distance = -1;
	new_record->line_integration_code = -1;
	new_record->station_code = -1;
	new_record->station_name_size = 0;
	new_record->station_name = NULL;
	new_record->line_name_size = 0;
	new_record->line_name = NULL;

    return new_record;
}
	

Record* tokenize_record(char *buffer) {
	Record* temp_record = (Record*)malloc(sizeof(Record));

	if(!temp_record) {
		return NULL;
	}

	char *token = strtok(buffer, ",");
	temp_record->station_code = integer_or_null(token);

	token = strtok(NULL, ",");
	temp_record->station_name = strdup(token);
	temp_record->station_name_size = strlen(token);

	token = strtok(NULL, ",");
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

void save_record_to_bin(FILE* bin_filename, Record* new_record) {
	long record_begin = ftell(bin_filename);

	save_record(bin_filename, new_record);

	fwrite(&new_record->station_name_size, sizeof(int), 1, bin_filename);

	if(new_record->station_name_size > 0) {
		fwrite(new_record->station_name, sizeof(char), new_record->station_name_size, bin_filename);
	}

	fwrite(&new_record->line_name_size, sizeof(int), 1, bin_filename);
	if(new_record->line_name_size > 0) {
		fwrite(new_record->line_name, sizeof(char), new_record->line_name_size, bin_filename);
	}

	long record_end = ftell(bin_filename);

	int remain_bytes = record_SIZE - (record_end - record_begin);

	if(remain_bytes > 0) {
		char empty = '$';
		for(int i = 0; i < remain_bytes; i++) fwrite(&empty, sizeof(char), 1, bin_filename);
	}

}

void save_record(FILE* bin_filename, Record* new_record) {
	
	fwrite(&new_record->removed, sizeof(char), 1, bin_filename);
	fwrite(&new_record->next_record, sizeof(int), 1, bin_filename);
	fwrite(&new_record->station_code, sizeof(int), 1, bin_filename);
	fwrite(&new_record->line_code, sizeof(int), 1, bin_filename);
	fwrite(&new_record->next_station_code, sizeof(int), 1, bin_filename);
	fwrite(&new_record->next_station_distance, sizeof(int), 1, bin_filename);
	fwrite(&new_record->line_integration_code, sizeof(int), 1, bin_filename);
	fwrite(&new_record->station_integration_code, sizeof(int), 1, bin_filename);
}

int write_record(char* filename, Record* new_record) {
	FILE* file = fopen(filename, READ_BINARY_MODE);

	// todo: implement the complete function
	
	fclose(file);
	return 0;
}

int read_record(FILE* bin_file, Record* bin_record){
	/*
        This function confirm if the record was successful read

        Args:
            (FILE*) bin_file: binary input file
            (Record*) bin_record: the record struct that will be read

        Return:
            0 for sucess and -1 for fail
    
    */
   	if(!bin_file) return -1;
	if(!bin_record) return -1;

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
	if(bin_record->station_name_size > 0){
		bin_record->station_name = malloc(bin_record->station_name_size);
		if(bin_record->station_name == NULL) return -1;
		verify += fread(bin_record->station_name, sizeof(char), bin_record->station_name_size, bin_file);
	}
	
	verify += fread(&bin_record->line_name_size, sizeof(int), 1, bin_file);
	if(bin_record->line_name_size > 0){
		bin_record->line_name = malloc(bin_record->line_name_size);
		if(bin_record->line_name == NULL) return -1;
		verify += fread(bin_record->line_name, sizeof(char), (bin_record->line_name_size), bin_file);
	}

	char trash[43];
	int trash_size = 43 - bin_record->station_name_size - bin_record->line_name_size;
	fread(trash, sizeof(char), trash_size, bin_file);

	if(verify == 10 + bin_record->station_name_size + bin_record->line_name_size) return 0;
    else return -1;
}

void print_int(int value){
	if(value == -1) printf("NULO ");
	else printf("%d ", value);
}

void print_string(char* string, int size){
	if(size == 0) printf("NULO ");
	else{
		printf("%.*s ", size, string);
	}
}

void print_record(Record* bin_record){
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

Record* read_record_RRN(char* filename, int RRN) {
	FILE* file = fopen(filename, READ_BINARY_MODE);


	fclose(file);
	return 0;

}

void free_record(Record** temp_record) {
	if(!temp_record) return;

	if((*temp_record)->station_name) {
		free((*temp_record)->station_name);
		(*temp_record)->station_name = NULL;
	}

	if((*temp_record)->line_name) {
		free((*temp_record)->line_name);
		(*temp_record)->line_name = NULL;
	}

	free(*temp_record);

	*temp_record = NULL;
}

