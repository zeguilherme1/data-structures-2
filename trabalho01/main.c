#include <stdio.h>
#include <string.h>
#include "register.h"
#include "header.h"

int csv_to_bin() {
	char csv_filename[100], bin_filename[100];

	scanf("%s %s", csv_filename, bin_filename);

	FILE *csv_file = fopen(csv_filename, "r");
	FILE *bin_file = fopen(bin_filename, READ_BINARY_MODE);

	Header* temp_header = create_header();

	if(!temp_header) return -1;

	save_header(bin_file, temp_header);
	
	char buffer[200]; // Removes the first CSV line
	fgets(buffer, sizeof(buffer), csv_file);

	int register_counter = 0;

	while(fgets(buffer, sizeof(buffer), csv_file)) {
		Record *new_register = tokenize_register(buffer);

		if(!new_register) return -1;

		new_register->removed = '0';
		new_register->next_register = temp_header->top;

		save_register_to_bin(bin_file, new_register);
		register_counter++;

		free_register(&new_register);
	}

	temp_header->status = '0';
	temp_header->nextRRN = register_counter;

	save_header(bin_file, temp_header);

	return 0;
}

int main() {

	



	return 0;
}
