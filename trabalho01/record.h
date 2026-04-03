#ifndef RECORD_H
#define RECORD_H

#define HEADER_SIZE 17
#define record_SIZE 80
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
	int next_record;
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

Record* new_record();

int write_record(char *filename, Record* new_record);

Record* tokenize_record(char *buffer);
void save_record_to_bin(FILE* bin_filename, Record* new_record);
void save_record(FILE* bin_filename, Record* new_record);
int read_record(FILE* bin_file, Record* bin_record);
void print_record(Record* bin_record);
void free_record(Record** temp_record);
Record* read_record_RRN(char* filename, int RRN);

#endif
