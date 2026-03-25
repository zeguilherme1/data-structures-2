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
int main() {
	

	return 0;
}
