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
	// Todo


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

Record* read_register_RRN(char* filename, int RRN) {
	FILE* file = fopen(filename, READ_BINARY_MODE);


	fclose(file);
	return 0;

}

