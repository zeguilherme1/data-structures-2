#ifndef REGISTER_H
#define REGISTER_H

#define HEADER_SIZE 17
#define REGISTER_SIZE 80
#define READ_BINARY_MODE "rb+"
#define WRITE_BINARY_MODE "wb+"

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include "header.h"

// we are using record instead of "register"
// because register is a reserved keyword in C

typedef struct record {
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
} Record;

int write_register(char *filename, Record* new_record);

Record* tokenize_register(char *buffer);
void save_register_to_bin(FILE* bin_filename, Record* new_register);
void save_register(FILE* bin_filename, Record* new_register);
Record* read_register_RRN(char* filename, int RRN);

#endif
