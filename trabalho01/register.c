#include <stdio.h>
#include "register.h"

struct record {
	char removed;
	int next_register;
	int station_code;
	int line_code;
	int next_station_code;
	int next_station_distance;
	int line_integration_code;
	int station_integration_code;

	int station_name_size;
	char* station_name;

	int line_name_size;
	char* line_name;
};

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
    new_record->next_register = -1;
    new_record->station_code = -1;
    new_record->line_code = -1;
    new_record-> next_station_code = -1;
	new_record-> next_station_distance = -1;
	new_record-> line_integration_code = -1;
	new_record-> station_integration_code = -1;
	new_record-> station_name_size = 0;
	new_record-> station_name = NULL;
	new_record-> line_name_size = 0;
	new_record-> line_name = NULL;

    return new_record;
}
	

Record* tokenize_register(char *buffer) {
	Record* temp_register = (Record*)malloc(sizeof(Record));

	if(!temp_register) {
		return NULL;
	}

	char *token = strtok(buffer, ',');
	if(!token) return NULL;
	temp_register->station_code = atoi(token);

	token = strtok(NULL, ',');
	temp_register->station_name = strdup(token);
	temp_register->station_name_size = strlen(token);

	token = strtok(NULL, ',');
	temp_register->line_name = strdup(token);
	temp_register->line_name_size = strlen(token);
	
	token = strtok(NULL, ',');
	if(!token) return NULL;
	temp_register->next_station_code = atoi(token);

	token = strtok(NULL, ',');
	if(!token) return NULL;
	temp_register->next_station_distance = atoi(token);

	token = strtok(NULL, ',');
	if(!token) return NULL;
	temp_register->line_integration_code = atoi(token);

	token = strtok(NULL, ',');
	if(!token) return NULL;
	temp_register->station_integration_code = atoi(token);

	return temp_register;
}

void save_register_to_bin(FILE* bin_filename, Record* new_register) {
	long register_begin = ftell(bin_filename);

	save_register(bin_filename, new_register);

	fwrite(&new_register->station_name_size, sizeof(int), 1, bin_filename);

	if(new_register->station_name_size > 0) {
		fwrite(new_register->station_name, sizeof(char), new_register->station_name_size, bin_filename);
	}

	fwrite(&new_register->line_name_size, sizeof(int), 1, bin_filename);
	if(new_register->line_name_size > 0) {
		fwrite(new_register->line_name, sizeof(char), new_register->line_name_size, bin_filename);
	}

	long register_end = ftell(bin_filename);

	int remain_bytes = REGISTER_SIZE - (register_end - register_begin);

	if(remain_bytes > 0) {
		char empty = '$';
		for(int i = 0; i < remain_bytes; i++) fwrite(&empty, sizeof(char), 1, bin_filename);
	}

}

void save_register(FILE* bin_filename, Record* new_register) {
	
	fwrite(&new_register->removed, sizeof(char), 1, bin_filename);
	fwrite(&new_register->next_register, sizeof(int), 1, bin_filename);
	fwrite(&new_register->station_code, sizeof(int), 1, bin_filename);
	fwrite(&new_register->line_code, sizeof(int), 1, bin_filename);
	fwrite(&new_register->next_station_code, sizeof(int), 1, bin_filename);
	fwrite(&new_register->next_station_distance, sizeof(int), 1, bin_filename);
	fwrite(&new_register->line_integration_code, sizeof(int), 1, bin_filename);
	fwrite(&new_register->station_integration_code, sizeof(int), 1, bin_filename);
}

int write_register(char* filename, Record* new_register) {
	FILE* file = fopen(filename, READ_BINARY_MODE);

	// todo: implement the complete function
	
	fclose(file);
	return 0;
}

int read_register(FILE* bin_file, Record* bin_register){
	/*
        This function confirm if the record was successful read

        Args:
            (FILE*) bin_file: binary input file
            (Record*) bin_register: the register struct that will be read

        Return:
            0 for sucess and -1 for fail
    
    */
   	if(!bin_file) return -1;
	if(!bin_register) return -1;

    int verify = 0;

    verify += fread(&bin_register->removed, sizeof(char), 1, bin_file);
	verify += fread(&bin_register->next_register, sizeof(int), 1, bin_file);
	verify += fread(&bin_register->station_code, sizeof(int), 1, bin_file);
	verify += fread(&bin_register->line_code, sizeof(int), 1, bin_file);
	verify += fread(&bin_register->next_station_code, sizeof(int), 1, bin_file);
	verify += fread(&bin_register->next_station_distance, sizeof(int), 1, bin_file);
	verify += fread(&bin_register->line_integration_code, sizeof(int), 1, bin_file);
	verify += fread(&bin_register->station_integration_code, sizeof(int), 1, bin_file);
	
	verify += fread(&bin_register->station_name_size, sizeof(int), 1, bin_file);
	if(bin_register->station_name_size > 0){
		bin_register->station_name = malloc(bin_register->station_name_size);
		if(bin_register->station_name == NULL) return -1;
		verify += fread(bin_register->station_name, sizeof(char), bin_register->station_name_size, bin_file);
	}
	
	verify += fread(&bin_register->line_name_size, sizeof(int), 1, bin_file);
	if(bin_register->line_name_size > 0){
		bin_register->line_name = malloc(bin_register->line_name_size);
		if(bin_register->line_name == NULL) return -1;
		verify += fread(bin_register->line_name, sizeof(char), (bin_register->line_name_size), bin_file);
	}

	char trash[43];
	int trash_size = 43 - bin_register->station_name_size - bin_register->line_name_size;
	fread(trash, sizeof(char), trash_size, bin_file);

	if(verify == 10 + bin_register->station_name_size + bin_register->line_name_size) return 0;
    else return -1;
}

void print_int(int value){
	if(value == -1) printf("NULO");
	else printf("%d ", value);
}

void print_string(char* string, int size){
	if(size == 0) printf("NULO");
	else{
		printf("%.*s ", size, string);
	}
}

void print_register(Record* bin_register){
	print_int(bin_register->station_code);
	print_string(bin_register->station_name, bin_register->station_name_size);
	print_int(bin_register->line_code);
	print_string(bin_register->line_name, bin_register->line_name_size);
	print_int(bin_register->next_station_code);
	print_int(bin_register->next_station_distance);
	print_int(bin_register->line_integration_code);
	print_int(bin_register->station_integration_code);
	printf("\n");
}

Record* read_register_RRN(char* filename, int RRN) {
	FILE* file = fopen(filename, READ_BINARY_MODE);


	fclose(file);
	return 0;

}

void free_register(Record** temp_register) {
	if(!temp_register) return;

	if((*temp_register)->station_name) {
		free((*temp_register)->station_name);
		(*temp_register)->station_name = NULL;
	}

	if((*temp_register)->line_name) {
		free((*temp_register)->line_name);
		(*temp_register)->line_name = NULL;
	}

	free(*temp_register);

	*temp_register = NULL;
}

